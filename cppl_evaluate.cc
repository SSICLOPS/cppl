// Copyright 2015-2018 RWTH Aachen University
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>
#include <string>

#include "ast.hh"
#include "ast_print_visitor.hh"
#include "ast_preprocessor_visitor.hh"
#include "ast_policy_compressor_visitor.hh"
#include "binary.hh"
#include "policy_header.hh"
#include "policy_stack.hh"
#include "relation_set.hh"
#include "variable_set.hh"

#include "equation_driver.hh"

#include "debug.hh"

#include "performance.h"

#include "simple_reason_printer.hh"
#include "false_reason_printer.hh"

namespace  { 
    const size_t SUCCESS = 0; 
    const size_t ERROR_IN_COMMAND_LINE = 1; 
    const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
    const size_t POLICY_ERROR = 3; 
}

string getFile(string filename)  {
    ifstream file(filename);
    if(!file.is_open())
        throw runtime_error("Can't open file "+ filename);
    //read the file into string
    stringstream strStream;
    strStream << file.rdbuf();
    file.close();
    return strStream.str();
}

int main (int argc, char *argv[])  {
    try  { 
        //default options that can be overwriten by the program options
        bool traceParsingEnabled = false;
        bool traceScanningEnabled = false;
        string ccpplFile;
        string variablesFile;
        string policyDefinitionFile;
        string runtimeVariablesFile;

		//reason related
		bool printReasonEnabled = false;
		string reasonFile;
        
        //Define and parse the program options
        namespace po = boost::program_options; 
        po::options_description desc("Options"); 
        desc.add_options() 
            ("help", "Print help message") 
            ("ccppl-file", po::value<string>(), "input file (compressed cppl file")
            ("policy-definition-file", po::value<string>(), "JSON file that represents the policy definition")
            ("variables-file", po::value<string>(), "JSON file with values for the variables of the policy definition")
            ("runtime-variables-file", po::value<string>(), "JSON file with values for the runtime variables used by the custom functions")
            ("trace-parsing,p", "Print the parser trace (for debugging)") 
            ("trace-scanning,s", "Print the scanning trace (for debugging)")
			("print-reason", po::value<string>(), "Output reasons, why the evaluation is true, to a JSON file");

        //all positional options should be translated into "input-file" options
        po::positional_options_description p;
        p.add("input-file", -1);

        po::variables_map vm; 
        try  { 
            po::store(po::command_line_parser(argc, argv).
                              options(desc).positional(p).run(), vm);

            //--help option 
            if(vm.count("help"))  { 
                std::cout << "Compact Privacy Policy Language Generator" << std::endl 
                    << desc << std::endl; 
                return SUCCESS; 
            } 

            //trace parsing options
            if(vm.count("trace-parsing"))  {
                traceParsingEnabled = true;
            }

            //trace scanning options
            if(vm.count("trace-scanning"))  {
                traceScanningEnabled = true;
            }

            //policy-definition-file
            if(vm.count("policy-definition-file"))  {
                policyDefinitionFile = vm["policy-definition-file"].as<string>();
            } else  {
                std::cerr << "No policy definition file parameter!" << std::endl;
                return ERROR_IN_COMMAND_LINE;
            }

            //variables-file
            if(vm.count("variables-file"))  {
                variablesFile = vm["variables-file"].as<string>();
            } else  {
                variablesFile = ""; //this will use default values for the variables
            }

            //runtime-variables-file
            if(vm.count("runtime-variables-file"))  {
                runtimeVariablesFile = vm["runtime-variables-file"].as<string>();
            } else  {
                std::cerr << "No runtime variables file parameter!" << std::endl;
                return ERROR_IN_COMMAND_LINE;
            }

            //ccppl input-file
            if(vm.count("ccppl-file"))  {
                ccpplFile = vm["ccppl-file"].as<string>();
            } else  {
                std::cerr << "No ccppl file!" << std::endl;
                return ERROR_IN_COMMAND_LINE;
            }

			if (vm.count("print-reason")) {
				reasonFile = vm["print-reason"].as<string>();
				printReasonEnabled = true;
			}

            po::notify(vm); // throws on error
        } 
        catch(po::error& e)  { 
            std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
            std::cerr << desc << std::endl; 
            return ERROR_IN_COMMAND_LINE; 
        } 

        //application code 
        
        //evaluate the binary policy representation
        #if DEBUG_POLICY_EVALUATION
            cout << "----------Evaluation----------" << endl;
        #endif

        //load the necessary files before measuring to compensate file caching
        //get the policy definition from a json file
        string policyDefinitionJsonString = getFile(policyDefinitionFile);
        //load the normal and runtime variables before the measuring to compensate file caching
        string variablesJsonString = getFile(variablesFile);
        string runtimeVariablesJsonString = getFile(runtimeVariablesFile);

        #if EVALUATION_OUTPUT
            performance_start_benchmark();
        #endif

        //contains all information of the policy definition
        PolicyDefinition evalPolicyDefinition(policyDefinitionJsonString);
        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Eval Policy Definition time");
            performance_start_benchmark();
        #endif
        if(variablesFile.compare("") != 0)  {
            evalPolicyDefinition.loadVariableValues(variablesJsonString);
        }
        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Eval Definition Load Variables time");
            performance_start_benchmark();
        #endif
        //also load the runtime variables for the custom function handler
        evalPolicyDefinition.loadRuntimeVariableValues(runtimeVariablesJsonString);
        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Eval Definition Load Runtime Variables time");
            performance_start_benchmark();
        #endif

        //generate the function handler that evaluates the user defined function
        FunctionHandler evalFunctionHandler(evalPolicyDefinition);

        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Eval Create Function Handler time");
            performance_start_benchmark();
        #endif

        Binary policyBinary(NULL);
        policyBinary.read_from_file(ccpplFile);

        PolicyHeader evalPolicyHeader(policyBinary);
        #if DEBUG_POLICY_EVALUATION
            cout << "Policy Version: " << evalPolicyHeader.version << endl;
        #endif

        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Eval Policy Header time");
            performance_start_benchmark();
        #endif

        //this just creates the objects (no parsing involved), since we need them to init the policy stack class
        VariableSet evalVariableSet(evalPolicyDefinition, evalFunctionHandler, policyBinary);
        RelationSet evalRelationSet(evalPolicyDefinition, evalVariableSet, policyBinary);

        //after the policy header s.t. the offset is rights
        PolicyStack evalPolicyStack(evalRelationSet, policyBinary, policyBinary.getPosition());
        #if DEBUG_POLICY_EVALUATION
            cout << "RelationSetOffset: " << evalPolicyStack.getRelationSetOffset() << endl;
        #endif

        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Eval Policy Stack time");
            performance_start_benchmark();
        #endif

        //parse the relation set to populate the internal relation set structure
        //necessary since we don't now the size of relation set and so not the variableSetOffset
        evalRelationSet.parse(evalPolicyStack.getRelationSetOffset(), evalPolicyStack.getNumberOfRelations());
        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Eval Relation Set Parsing time");
            performance_start_benchmark();
        #endif

        //set the variable offset
        evalVariableSet.parse(evalRelationSet.getVariableSetOffset(), evalRelationSet.getNumberOfVariables());
        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Eval Variable Set Parsing time");
            performance_start_benchmark();
        #endif

        //process relation set s.t. we now know for every relation id the boolean result of it
        evalRelationSet.process();
        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Eval Relation Processing time");
            performance_start_benchmark();
        #endif

        bool policyResult = evalPolicyStack.processStack(evalRelationSet);

        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Eval Stack Processing time");
        #endif

        cout << "Policy result: " << (policyResult ? "true" : "false") << endl;

#ifndef PRINT_REASON_TO_CONSOLE
		if (printReasonEnabled){
#endif
			if (policyResult == true){
				SimpleReasonPrinter simpleReasonPrinter;
				simpleReasonPrinter.init(&evalPolicyDefinition, &evalRelationSet, &evalVariableSet);
				evalPolicyStack.printReason(simpleReasonPrinter);
//#ifdef CONVERT_TO_DNF
				//cout<<"------------------------------"<<endl;
				//simpleReasonPrinter.printDNF();
//#endif

#ifdef PRINT_REASON_TO_CONSOLE
				cout<<"------------------------------"<<endl;
				cout << simpleReasonPrinter.printReasonToJSON()<<endl;
				cout <<"------------------------------"<<endl;
				if (printReasonEnabled){
#endif //PRINT_REASON_TO_CONSOLE
				simpleReasonPrinter.printReasonToJSON(reasonFile);
#ifdef PRINT_REASON_TO_CONSOLE
				}
#endif
			}
			else {
				FalseReasonPrinter falseReasonPrinter;
				falseReasonPrinter.init(&evalPolicyDefinition, &evalPolicyStack, &evalRelationSet, &evalVariableSet);
				evalPolicyStack.printReason(falseReasonPrinter);
#ifdef CONVERT_TO_DNF
				cout<<"------------------------------"<<endl;
				falseReasonPrinter.printDNF();
#endif
				cout<<"------------------------------"<<endl;
				cout<<"The following formula cannot be satisfied:"<<endl;
				cout<<falseReasonPrinter.printReasonToString();
				cout<<"------------------------------"<<endl;
			}
		}

#ifndef PRINT_REASON_TO_CONSOLE
    } 
#endif
    catch(char const *msg)  {
        std::cerr << "Policy Error: " << msg << std::endl;
        return POLICY_ERROR;
    }
    catch(std::exception& e)  { 
        std::cerr << "Unhandled Exception reached the top of main: " 
            << e.what() << ", application will now exit" << std::endl; 
        return ERROR_UNHANDLED_EXCEPTION; 

    } 

    return SUCCESS; 
}

