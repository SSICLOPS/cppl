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

#include <iostream>
#include <fstream>
#include <string>

#include "ast_2_ccppl.hh"

#include "ast.hh"
#include "ast_preprocessor_visitor.hh"
#include "ast_2_policy_stack.hh"
#include "policy_definition.hh"
#include "relation_set.hh"
#include "ccppl_compressor.hh"

#include "equation_driver.hh"

//#include "debug.hh"

//#include "performance.h"

//#include "libcppl_compress.hh"

string Ast2Ccppl::getFile(string filename)  {
    ifstream file(filename);
    if(!file.is_open())
        throw runtime_error("Can't open file "+ filename);
    //read the file into string
    stringstream strStream;
    strStream << file.rdbuf();
    file.close();
    return strStream.str();
}

Binary * Ast2Ccppl::_Compress(std::string uncompressedCppl,
		const PolicyDefinition & policyDefinition,
        bool traceParsingEnabled,
        bool traceScanningEnabled)
{


	Binary * pBinary = NULL;
	try{
        //application code 
        
        //generate the function handler that evaluates the user defined function
		pBinary = new Binary(NULL);
        Binary & genBinary = *(pBinary);

        EquationDriver eqDriver;

        eqDriver.trace_parsing = traceParsingEnabled;
        eqDriver.trace_scanning = traceScanningEnabled;

        eqDriver.parse_stream(uncompressedCppl);

        //show an error if the policy is empty
        if(eqDriver.ast == NULL)  {
            cerr << "Policy is empty" << endl;
			delete pBinary;
            return NULL;
        }

        //preprocess the AST
        // - check that the types inside the AST are used correctly
        // - sort the children s.t. a correct PN is created by a DFS
        AstPreprocessorVisitor preprocessor = AstPreprocessorVisitor(policyDefinition);
        preprocessor.visit(*(eqDriver.ast));
        //compress the AST as RPN stack with equation set
		Ast2PolicyStack ast2PolicyStack(_policyStack, _relationSet);
        ast2PolicyStack.visit(*(eqDriver.ast));
		CcpplCompressor compressor;
		compressor.compress(genBinary, &policyDefinition, _policyStack, _relationSet);
		return &genBinary;
    } 
    catch(char const *msg)  {
        std::cerr << "Policy Error: " << msg << std::endl;
		if (pBinary != NULL)
			delete pBinary;
		return NULL;
    }
    catch(std::exception& e)  { 
        std::cerr << "Unhandled Exception reached the top of main: " 
            << e.what() << ", application will now exit" << std::endl; 
		if (pBinary != NULL)
			delete pBinary;
		return NULL;
    } 
}

Binary * Ast2Ccppl::compress(std::string uncompressedCppl,
		std::string policyDefinitionFile,
        bool traceParsingEnabled,
        bool traceScanningEnabled)
{
        //load the necessary files before measuring to compensate file caching
        //get the policy definition from a json file
        string policyDefinitionJsonString = getFile(policyDefinitionFile);

        //contains all information of the policy definition
        PolicyDefinition policyDefinition;
		policyDefinition.load(policyDefinitionJsonString);

	return  _Compress(uncompressedCppl, policyDefinition, traceParsingEnabled, traceScanningEnabled);
}

Binary * Ast2Ccppl::compress(string inputFile,
		const PolicyDefinition & policyDefinition,
		bool traceParsingEnabled,
		bool traceScanningEnabled)
{
	return _Compress(inputFile, policyDefinition, traceParsingEnabled, traceScanningEnabled);
}

bool Ast2Ccppl::compressToFile(std::string uncompressedCppl,
		std::string policyDefinitionFile,
		std::string outputFile,
        bool traceParsingEnabled,
        bool traceScanningEnabled)
{
	Binary * pb = compress(uncompressedCppl, policyDefinitionFile, traceParsingEnabled, traceScanningEnabled);
	if (pb != NULL){
		pb->write(outputFile);
		delete pb;
		return true;
	}
	else
		return false;
}

bool Ast2Ccppl::compressToFile(std::string uncompressedCppl,
		const PolicyDefinition & policyDefinition,
		std::string outputFile,
        bool traceParsingEnabled,
        bool traceScanningEnabled)
{
	Binary * pb = compress(uncompressedCppl, policyDefinition, traceParsingEnabled, traceScanningEnabled);
	if (pb != NULL){
		pb->write(outputFile);
		delete pb;
		return true;
	}
	else
		return false;
}
