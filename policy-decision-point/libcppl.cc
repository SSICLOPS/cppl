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

#include<fstream>
#include<sstream>
#include<stdlib.h>
//#include<regex>

#include "libcppl.hh"
#include "simple_reason_printer.hh"
#include "false_reason_printer.hh"

string temp_folder_path = "";

std::string getFileContent(const std::string & filePath){
	std::ifstream file(filePath);
	if (!file.is_open())
		throw "Can't open file" + filePath; 

	std::stringstream strStream;
	strStream << file.rdbuf();
	file.close();
	return strStream.str();
}

int cppl_init(){
	if (temp_folder_path != "")
		return 0;

	char temp[] = "/tmp/cppl_XXXXXX";
	char * str = mkdtemp(temp);
	if (str){
		temp_folder_path = str;
		if (temp_folder_path[temp_folder_path.size() - 1] != '/')
			temp_folder_path += '/';
		return 0;
	}
	else{
		cout<<"failed to create tmp directory\n";
		return 1;
	}
}

int cppl_cleanup(){
	//std::regex tmp("^/tmp.*");
	size_t found = temp_folder_path.find("/tmp/cppl_");
	if (found != std::string::npos && found == 0){//make sure the tragedy like, sudo rm -rf /, will never happen
		std::string rmCommand = "rm -rf " + temp_folder_path;
		int ret = system(rmCommand.c_str());
		if (ret) {
			cout<<"WARNING: failed to remove '"<<temp_folder_path<<"' (ret: "<<ret<<")"<<endl;
		}
		temp_folder_path = "";
	}
	return 0;
}

string cppl_read_policy_from_file(const string & cpplFilePath){
	return getFileContent(cpplFilePath);
}

bool cppl_compress_policy_to_file(const string & policy,
		const PolicyDefinition & policyDefinition,
		const string & outputFile,
        bool traceParsingEnabled,
        bool traceScanningEnabled)
{
	Binary * binary = cppl_compress_policy(policy, policyDefinition, traceParsingEnabled, traceScanningEnabled);
	if (binary){
		binary->write(outputFile);
		return true;
	}
	else{
		return false;
	}
	delete binary;
}

Binary * cppl_compress_policy(const string & policy,
		const PolicyDefinition & policyDefinition,
        bool traceParsingEnabled,
        bool traceScanningEnabled)
{
	PolicyStackCompress psc;
	return psc.compress(policy, policyDefinition, traceParsingEnabled, traceScanningEnabled);
}

PolicyDefinition * cppl_read_policy_definition_from_file(const string & policyDefinitionFile,
		const string & functionHandlerLibPath)
{
	PolicyDefinition * policyDefinition = new PolicyDefinition();
	policyDefinition->load(getFileContent(policyDefinitionFile), functionHandlerLibPath);

	return policyDefinition;
}

PolicyDefinition * cppl_read_policy_definition(const string & policyDefinition,
        const Binary * functionHandler){
	string libPath = "";
	if (functionHandler != NULL){
		//std::regex version_regex("\"version\"\\s*:\\s*([0-9]+),");
		//std::smatch match;
		//std::regex_search(policyDefinition, match, version_regex);
		size_t found = policyDefinition.find("\"version\"");
		if (found != std::string::npos){
			std::string versionID = "";
			for (;policyDefinition[found] < '0' || policyDefinition[found] > '9'; ++found);
			for (;policyDefinition[found] >= '0' && policyDefinition[found] <= '9';++found){
				versionID += policyDefinition[found];
			}
			libPath = temp_folder_path + "func_handler_" + versionID + ".so";
			functionHandler->write(libPath);
		}
	}

	PolicyDefinition * pd = new PolicyDefinition();
	pd->load(policyDefinition, libPath);

	return pd;
}

NodeParameters * cppl_read_node_parameters_from_file(const string & nodeParamsPath,
		const string & nodeRuntimeParamsPath,
		const PolicyDefinition * policyDefinition)
{
	return cppl_read_node_parameters(getFileContent(nodeParamsPath), getFileContent(nodeRuntimeParamsPath), policyDefinition);
}

NodeParameters * cppl_read_node_parameters(const string & nodeParams,
        const string & nodeRuntimeParams,
        const PolicyDefinition * policyDefinition){
	
	std::string concatedNodeParames = "[" 
		+ nodeParams 
		+ "," 
		+ nodeRuntimeParams
		+"]";

	NodeParameters * np = new NodeParameters();
	np->load(concatedNodeParames, policyDefinition);

	return np;
}

PolicyStack * cppl_read_compressed_cppl_from_file(const string & ccpplFilePath,
		const PolicyDefinition * policyDefinition)
{
	Binary policyBinary(NULL);
	policyBinary.read_from_file(ccpplFilePath);

	PolicyStack * policyStack = new PolicyStack();
	policyStack->load(policyBinary, policyDefinition);

	return policyStack;
}

PolicyStack * cppl_read_compressed_cppl(Binary & binary,
		const PolicyDefinition * policyDefinition)
{
	PolicyStack * policyStack = new PolicyStack();
	policyStack->load(binary, policyDefinition);

	return policyStack;
}

bool cppl_evaluate(PolicyStack * policyStack, const NodeParameters * NodeParameters){
	policyStack->setNodeParameters(NodeParameters);
	return policyStack->doEval();
}

string cppl_get_reason(PolicyStack * policyStack, const PolicyDefinition * policyDefinition){
	if (policyStack->getResult()){
		SimpleReasonPrinter simpleReasonPrinter;
		simpleReasonPrinter.init(policyDefinition,&policyStack->getRelationSet());
		policyStack->printReason(simpleReasonPrinter);
		return simpleReasonPrinter.printReasonToJSON();
	}
	else{
		FalseReasonPrinter falseReasonPrinter;
		falseReasonPrinter.init(policyDefinition, &policyStack->getStack(), &policyStack->getRelationSet());
		policyStack->printReason(falseReasonPrinter);
		return falseReasonPrinter.printReasonToString();
	}
}
