#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>
#include <string>
//#include <regex>

#include "binary.hh"
#include "policy_definition.hh"
#include "node_parameter.hh"
#include "policy_stack.hh"
#include "libcppl.hh"
#include "simple_reason_printer.hh"
#include "false_reason_printer.hh"
#include "performance.h"

#include "options.hh"

namespace  { 
    const size_t SUCCESS = 0; 
    const size_t ERROR_IN_COMMAND_LINE = 1; 
    const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
    const size_t POLICY_ERROR = 3; 
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
		string functionHandlerLib;

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
			("function-handler", po::value<string>(), "lib of function handler of policy definition")
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
			
			//function-handler
            if(vm.count("function-handler"))  {
                functionHandlerLib = vm["function-handler"].as<string>();
            } else  {
                std::cerr << "No function handler parameter!" << std::endl;
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
#if DEBUG_POLICY_EVALUATION
		std::cout<< "----------Evaluation----------"<<std::endl;
#endif

		cppl_init();

        // Although we could use cppl_read_policy_definition_from_file(), we
        // first read files into memory to test cppl_read_policy_definition()
        Binary binary;
        binary.read_from_file(functionHandlerLib);
        string policyDef = cppl_read_policy_from_file(policyDefinitionFile);
#if EVALUATION_OUTPUT
		performance_start_benchmark();
#endif
	    PolicyDefinition * evalPolicyDefinition = cppl_read_policy_definition(policyDef, &binary);	
		//PolicyDefinition * evalPolicyDefinition = cppl_read_policy_definition_from_file(policyDefinitionFile, functionHandlerLib);
#if EVALUATION_OUTPUT
		performance_stop_benchmark("Eval load Policy Definition time");
		performance_start_benchmark();
#endif
		NodeParameters * evalNodeParameters = cppl_read_node_parameters_from_file(variablesFile, runtimeVariablesFile, evalPolicyDefinition);
#if EVALUATION_OUTPUT
		performance_stop_benchmark("Eval load node parameter time");
		performance_start_benchmark();
#endif
		PolicyStack * evalPolicyStack = cppl_read_compressed_cppl_from_file(ccpplFile, evalPolicyDefinition);
#if EVALUATION_OUTPUT
		performance_stop_benchmark("Eval load compressed cppl time");
		performance_start_benchmark();
#endif
		bool result = cppl_evaluate(evalPolicyStack, evalNodeParameters);
#if EVALUATION_OUTPUT
		performance_stop_benchmark("Eval evaluation time");
#endif
		std::cout<< "Policy result: " << ((result)?"true\n":"false\n") << endl;
		if (printReasonEnabled){
			std::string reason = cppl_get_reason(evalPolicyStack, evalPolicyDefinition);
			std::cout<<reason<<std::endl;
			if (result){
				std::ofstream out(reasonFile);
				out<<reason;
				out.close();
			}
		}

		delete evalPolicyStack;
		delete evalNodeParameters;
		delete evalPolicyDefinition;
	} 
    catch(char const *msg)  {
        std::cerr << "Policy Error: " << msg << std::endl;
        return POLICY_ERROR;
    }
	//catch(const std::regex_error& e){
		//std::cout << "regex_error caught: " << e.what() << '\n';
	//}
	catch(std::exception& e)  { 
        std::cerr << "Unhandled Exception reached the top of main: " 
            << e.what() << ", application will now exit" << std::endl; 
        return ERROR_UNHANDLED_EXCEPTION; 

    } 

	cppl_cleanup();

    return SUCCESS; 
}

