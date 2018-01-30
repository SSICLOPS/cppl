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

#pragma once

#include <stdint.h>
#include <vector>
#include <cstring>


typedef uint16_t package_type_t;
typedef uint16_t data_len_t;
typedef uint16_t policy_len_t;

enum PackageHeaderFieldLen{
	TYPE = sizeof(package_type_t),
	DATA_LEN = sizeof(data_len_t),
	POLICY_LEN = sizeof(policy_len_t),
	COMMON_HEADER_LEN = TYPE + DATA_LEN,
	POLICY_ENABLED_DATA_HEADER_LEN = COMMON_HEADER_LEN + POLICY_LEN
};

enum PackageType{
	NODE_PARAM = 1,
	POLICY_ENABLED_DATA = 2
};

//std::vector<char> makePolicyEnabledDataPackage(const void * data, size_t dataLen, const void * ccppl, size_t ccpplLen){
	//size_t packageLen = PackageHeaderFieldLen::POLICY_ENABLED_DATA_HEADER_LEN + dataLen + ccpplLen;
	//std::vector<char> package(packageLen);

	//*reinterpret_cast<package_type_t *>(&package[0]) = PackageType::POLICY_ENABLED_DATA;
	//*reinterpret_cast<data_len_t *>(&package[0] + PackageHeaderFieldLen::TYPE) = dataLen;
	//*reinterpret_cast<policy_len_t *>(&package[0] + PackageHeaderFieldLen::COMMON_HEADER_LEN) = ccpplLen;

	//char * pData = &package[0] + PackageHeaderFieldLen::POLICY_ENABLED_DATA_HEADER_LEN;
	//char * pCcppl = (&package[0] + PackageHeaderFieldLen::POLICY_ENABLED_DATA_HEADER_LEN + dataLen);

	//memcpy(pData, data, dataLen);
	//memcpy(pCcppl, ccppl, ccpplLen);

	//return package;
//}

//std::vector<char> makeNodeParamsPackage(const std::string & nodeParams, const std::string & runtimePrams){
	//size_t packageLen = PackageHeaderFieldLen::TYPE + nodeParams.size() + runtimePrams.size() + 3;
//}

//inline package_type_t * getTypeField(void * buf){
	//return reinterpret_cast<package_type_t *>(buf);
//}

//inline data_len_t * getDataLengthField(void * buf){
	//return reinterpret_cast<data_len_t *>((char *)buf + PackageHeaderFieldLen::TYPE);
//}

//inline policy_len_t * getPolicyLengthField(void * buf){
	//return reinterpret_cast<policy_len_t *>((char *)buf + PackageHeaderFieldLen::COMMON_HEADER_LEN);
//}
