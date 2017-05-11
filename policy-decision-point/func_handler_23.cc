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
