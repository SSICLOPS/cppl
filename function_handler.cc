#include "function_handler.hh"

bool FunctionHandler::processFunction(const string &funcName, const vector<Variable> &funcParams)  {
    if(funcName.compare("storage.deleteAfter") == 0)  {
        if(policyDefinition.getIdValue("storage.canDelete").asBoolean == false)
            return false;

        if(policyDefinition.getRuntimeValue("earliestDeleteTime").asInt32 > funcParams.at(0).value.asInt32)
            return false;

        if(policyDefinition.getRuntimeValue("latestDeleteTime").asInt32 <= funcParams.at(0).value.asInt32)
            return false;

        return true;
    }
    else if(funcName.compare("storage.deleteBefore") == 0)  {
        if(policyDefinition.getIdValue("storage.canDelete").asBoolean == false)
            return false;

        if(policyDefinition.getRuntimeValue("earliestDeleteTime").asInt32 > funcParams.at(0).value.asInt32)
            return false;

        if(policyDefinition.getRuntimeValue("latestDeleteTime").asInt32 <= funcParams.at(0).value.asInt32)
            return false;

        return true;
    }
    else if(funcName.compare("sendEmail") == 0)  {
        if(policyDefinition.getIdValue("canSendEmail").asBoolean == false)
            return false;

        return true;
    }
    else if(funcName.compare("log") == 0)  {
        if(policyDefinition.getIdValue("canLog").asBoolean == false)
            return false;

        if(policyDefinition.getRuntimeValue("canLogWithin5Min").asBoolean == false)
            return false;

        return true;
    }
    else if(funcName.compare("logIn2") == 0)  {
        if(policyDefinition.getIdValue("canLog").asBoolean == false)
            return false;

        if(policyDefinition.getRuntimeValue("canLogWithin2Min").asBoolean == false)
            return false;

        return true;
    }
    else if(funcName.compare("logWithin") == 0)  {
        if(policyDefinition.getIdValue("canLog").asBoolean == false)
            return false;

        if((funcParams.at(0).value.asString)->compare("contact") == 0)  {
            if(policyDefinition.getRuntimeValue("lastestContactLogTime").asInt32 > funcParams.at(1).value.asInt32)
                return false;
        }
        else if((funcParams.at(0).value.asString)->compare("delivery") == 0)  {
            if(policyDefinition.getRuntimeValue("lastestDeliveryLogTime").asInt32 > funcParams.at(1).value.asInt32)
                return false;
        }
        else if((funcParams.at(0).value.asString)->compare("pseudo-analysis") == 0)  {
            if(policyDefinition.getRuntimeValue("lastestPseudoAnalysisLogTime").asInt32 > funcParams.at(1).value.asInt32)
                return false;
        }
        else  {
            throw "Unsupported action in log within time.";
        }

        return true;
    }
    else if(funcName.compare("accessFrom") == 0)  {
        return true;
    }
    else if(funcName.compare("accessUntil") == 0)  {
        return true;
    }
    else if(funcName.compare("notify") == 0)  {
        if(policyDefinition.getRuntimeValue("canNotify").asBoolean == false)
            return false;

        return true;
    }
    else if(funcName.compare("storage.backupHistory") == 0) {
        if(policyDefinition.getIdValue("storage.canBackupHistory").asBoolean == false) {
            return false;
        }

        return true;
    }
    else
        throw "Unsupported function in function handler. Please, implement this function in the function handler.";
}
