import json
import os

variables={"provider": [1, "CompanyA", "CompanyB"],
           "log_access": [1, True, False],
           "canDelete": [1, True, False],
           "canBackupHistory": [1, True, False],
           "location": [1, "DE", "FR", "US", "GB", "NL", "EU"],
           "encryption": [1, True, False],
           "replication": [1, 0, 5],
           "deleteTime": [1, (0,0), (1735693100, 1735693300),(1735693301, 1735694000)]
        }

def next():
    for key, val in variables.items():
        if val[0] == len(val) - 1:
            val[0] = 1
        else:
            val[0] += 1
            return True

    return False

def get_value(str):
    return variables[str][variables[str][0]]

if not os.path.exists("generated_test_cases"):
    os.makedirs("generated_test_cases")

testCaseID = 0
while True:
    fileNodeParams = open("generated_test_cases/policy_nodeparams_" + str(testCaseID) + ".json", "w+")
    fileNodeParams.write(json.dumps({ 
        "variables": {                                                                                                 
            "storage.provider": get_value("provider"),
            "storage.log_access": get_value("log_access"),
            "storage.canDelete": get_value("canDelete"),
            "storage.canBackupHistory": get_value("canBackupHistory"),
            "storage.location": get_value("location"),
            "storage.encryption": get_value("encryption"),
            "storage.replication": get_value("replication")
            }
        }
        , indent=4))
    fileNodeParams.close()

    fileNodeRuntimeParams = open("generated_test_cases/policy_noderuntimeparams_" + str(testCaseID) + ".json", "w+")
    fileNodeRuntimeParams.write(json.dumps({ 
        "variables": [
            {
                "name": "earliestDeleteTime",
                "type": "int32",
                "value": get_value("deleteTime")[0]
                },
            {
                "name": "latestDeleteTime",
                "type": "int32",
                "value": get_value("deleteTime")[1]
                }
            ]
        }
        , indent = 4))
    fileNodeRuntimeParams.close()

    if (next() == False):
        break

    testCaseID += 1
