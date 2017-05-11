#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>
#include <string>

#include "libcppl.hh"
#include "performance.h"


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
        string inputFile;
        string policyDefinitionFile;
        string outputFile;
        
        //Define and parse the program options
        namespace po = boost::program_options; 
        po::options_description desc("Options"); 
        desc.add_options() 
            ("help", "Print help message") 
            ("input-file", po::value<string>(), "input file")
            ("policy-definition-file", po::value<string>(), "JSON file that represents the policy definition")
            ("output-file,o", po::value<string>(), "Writes the compressed policy to this file (example: $(basename <input-file>).ccppl)")
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

            //output-file
            if(vm.count("output-file"))  {
                outputFile = vm["output-file"].as<string>();
            } else  {
                std::cerr << "No output file!" << std::endl;
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
#if EVALUATION_OUTPUT
		performance_start_benchmark();
#endif
		PolicyDefinition * policyDefinition = cppl_read_policy_definition_from_file(policyDefinitionFile);
		cppl_compress_policy_to_file(cppl_read_policy_from_file(inputFile), *policyDefinition, outputFile, traceParsingEnabled, traceScanningEnabled);
#if EVALUATION_OUTPUT
		performance_stop_benchmark("Gen compressor time");
#endif
        
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

