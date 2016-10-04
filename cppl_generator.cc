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
        string inputFile;
        string variablesFile;
        string policyDefinitionFile;
        string runtimeVariablesFile;
        
        //Define and parse the program options
        namespace po = boost::program_options; 
        po::options_description desc("Options"); 
        desc.add_options() 
            ("help", "Print help message") 
            ("input-file", po::value<string>(), "input file")
            ("policy-definition-file", po::value<string>(), "JSON file that represents the policy definition")
            ("variables-file", po::value<string>(), "JSON file with values for the variables of the policy definition")
            ("runtime-variables-file", po::value<string>(), "JSON file with values for the runtime variables used by the custom functions")
            ("trace-parsing,p", "Print the parser trace (for debugging)") 
            ("trace-scanning,s", "Print the scanning trace (for debugging)");

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

            //input-file
            if(vm.count("input-file"))  {
                inputFile = vm["input-file"].as<string>();
            } else  {
                std::cerr << "No input file!" << std::endl;
                return ERROR_IN_COMMAND_LINE;
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

            po::notify(vm); // throws on error
        } 
        catch(po::error& e)  { 
            std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
            std::cerr << desc << std::endl; 
            return ERROR_IN_COMMAND_LINE; 
        } 

        //application code 
        
        //load the necessary files before measuring to compensate file caching
        //get the policy definition from a json file
        string policyDefinitionJsonString = getFile(policyDefinitionFile);

        #if EVALUATION_OUTPUT
            performance_start_benchmark();
        #endif

        //contains all information of the policy definition
        PolicyDefinition policyDefinition(policyDefinitionJsonString);
        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Gen Policy Definition time");
            performance_start_benchmark();
        #endif

        //generate the function handler that evaluates the user defined function
        FunctionHandler functionHandler(policyDefinition);

        Binary genBinary(NULL);
        VariableSet variableSet(policyDefinition, functionHandler, genBinary);
        RelationSet relationSet(policyDefinition, variableSet, genBinary);
        PolicyStack policyStack(relationSet, genBinary, 0);

        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Gen Policy Class init time");
            performance_start_benchmark();
        #endif

        EquationDriver eqDriver;

        eqDriver.trace_parsing = traceParsingEnabled;
        eqDriver.trace_scanning = traceScanningEnabled;

        eqDriver.parse(inputFile);

        //show an error if the policy is empty
        if(eqDriver.ast == NULL)  {
            cerr << "Policy is empty" << endl;
            return POLICY_ERROR;
        }

        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Gen Policy Parsing time");
            performance_start_benchmark();
        #endif

        //preprocess the AST
        // - check that the types inside the AST are used correctly
        // - sort the children s.t. a correct PN is created by a DFS
        AstPreprocessorVisitor preprocessor = AstPreprocessorVisitor(policyDefinition);
        preprocessor.visit(*(eqDriver.ast));

        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Gen AST Preprocessing time");
            performance_start_benchmark();
        #endif

        //print the AST
        #if DEBUG_AST_PRINT
            AstPrintVisitor printVisitor = AstPrintVisitor();
            printVisitor.visit(*(eqDriver.ast));
        #endif

        //generate the policy header
        PolicyHeader policyHeader(genBinary, policyDefinition.getVersion());

        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Gen Policy Header time");
            performance_start_benchmark();
        #endif

        //compress the AST as RPN stack with equation set
        AstPolicyCompressorVisitor policyCompressor = AstPolicyCompressorVisitor(policyDefinition, policyStack, relationSet);
        #if DEBUG_POLICY_GENERATION
            cout << "Compressing the policy: " << endl;
        #endif
        policyCompressor.visit(*(eqDriver.ast));
        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Gen Policy Compressor time");
            performance_start_benchmark();
        #endif
        #if DEBUG_POLICY_GENERATION
            cout << endl;
        #endif

        //add the policy parts in the right order to the binary
        policyHeader.store();
        policyStack.store();
        relationSet.store();
        variableSet.store();
        #if EVALUATION_OUTPUT
            performance_stop_benchmark("Gen Binary Creation time");
        #endif

        //DEBUG output for PolicyStack sizes
        #if EVALUATION_OUTPUT 
            policyHeader.printSize();
            policyStack.printSize();
            relationSet.printSize();
            variableSet.printSize();
        #endif

        #if DEBUG_POLICY_GENERATION
            //debug output for the complete policy parts
            cout << "PolicyHeader: " << policyHeader.legend << endl << policyHeader << endl;
            cout << "PolicyStack: "  <<  policyStack.legend << endl << policyStack << endl;
            cout << "RelationSet: "  <<  relationSet.legend << relationSet << endl;
            cout << "VariableSet: "  <<  variableSet.legend << variableSet << endl;
        #endif

        #if DEBUG_POLICY_GENERATION
            cout << "Policy: "<< endl;
            genBinary.print();
        #endif
        #if EVALUATION_OUTPUT
            cout << "Policy length: " << genBinary.size() << endl;
        #endif

        uint8_t *policyMem = genBinary.getPointer();
        
        //evaluate the binary policy representation
        #if DEBUG_POLICY_EVALUATION
            cout << "----------Evaluation----------" << endl;
        #endif

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

        Binary policyBinary(policyMem);

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
    } 
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

