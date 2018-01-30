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

#include<vector>

#include "variable.hh"
#include "basic_type.hh"
#include "node_parameter.hh"

extern "C" {
Variable * storage_deleteAfter(const std::vector<Variable *> & funcParameters, const NodeParameters * nodeParameters);
Variable * storage_backupHistory(const std::vector<Variable *> & funcParameters, const NodeParameters * nodeParameters);
}

Variable * storage_deleteAfter(const std::vector<Variable *> & funcParameters, const NodeParameters * nodeParameters){
	Boolean * b = new Boolean(false);
	
	if (! nodeParameters->getParameter(2)->isTrue())//storage.canDelete
		return b;

	if (*(nodeParameters->getRuntimeParameter("earliestDeleteTime")) > *funcParameters.at(0))
		return b;

	if (*(nodeParameters->getRuntimeParameter("latestDeleteTime")) <= *funcParameters.at(0))
		return b;

	b->set_value(true);
	return b;
}

Variable * storage_backupHistory(const std::vector<Variable *> & funcParameters, const NodeParameters * nodeParameters){
	Boolean * b = new Boolean(false);

	if (! nodeParameters->getParameter(4)->isTrue())//storage.canBackupHistory
		return b;

	b->set_value(true);
	return b;
}
