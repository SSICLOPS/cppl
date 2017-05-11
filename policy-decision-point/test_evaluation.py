import json
import sys

def storage_deleteAfter(param, runtime, time):
    if param["storage.canDelete"] == False:
        return False

    if runtime[0]["value"] > time:#earliestDeleteTime
        return False

    if runtime[1]["value"] <= time:#latestDeleteTime
        return False

    return True

def storage_backupHistory(param, runtime, time):
    if param["storage.canBackupHistory"] == False:
        return False

    return True

node_parameter_data = open(sys.argv[1])
runtime_parameter_data = open(sys.argv[2])

node_parameter = json.load(node_parameter_data)["variables"]
runtime_parameter = json.load(runtime_parameter_data)["variables"]

result = (node_parameter["storage.provider"] != "CompanyA")\
        and (node_parameter["storage.log_access"] == True)\
        and (storage_deleteAfter(node_parameter, runtime_parameter, 1735693210))\
        and (storage_backupHistory(node_parameter, runtime_parameter, "1M"))\
        and (node_parameter["storage.location"] == "DE"\
            or (node_parameter["storage.location"] == "EU" and node_parameter["storage.encryption"] == True))\
        and (node_parameter["storage.replication"] >= 2)

if result:
    print("true")
else:
    print("false")
