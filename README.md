# CPPL - Compact Privacy Policy Language

## Versions

This repository contains two versions of CPPL. The first is the version used for the original paper at ACM WPES 2016 with some extensions. The second version is the library form libcppl which is built in `policy-decision-point` and provided the basis for several integration concepts presented in the paper at IEEE IC2E 2018. We suggest the use of the latter if not explicitly pointed to otherwise.

## Build Instructions

### Preliminaries

Tested with Ubuntu 14.04

```bash
sudo apt-get install g++ flex bison libboost-all-dev libjsoncpp-dev
```

### Configuration

See `debug.hh` and `options.hh`, e.g., ```#define DEBUG_POLICY_GENERATION 1``` enables output of the policy generation including the compressed policy.
For libcppl see the corresponding files in `policy-decision-point`.

### Compiling and Testing

```bash
make
make test
```

#### Integrating CPPL in your python project

If you want to use CPPL from a python project you may use the available python module, which is built with
```bash
make python-module
```
This will produce `cppl_cpp_python_bridge.so` and `cppl.py` in `python-module/build/lib.<architecture>`.
You should only use `cppl.py` in your python project, but still need to make `cppl_cpp_python_bridge.so` and `policy-decision-point/libcppl.so` available for the python interpreter, e.g., by setting `LD_LIBRARY_PATH` accordingly.

#### Troubeshooting

If your system provides the jsoncpp library in version smaller 0.7.0 the compiling process will encounter errors such as ```undefined reference to `Json::Value::isInt64() const'```. In such a case, please set `USE_SYSTEM_JSONCPP` in Makefile and policy-decision-point/Makefile to 0 to use the jsoncpp version included in this repository.

## Usage Examples

For additional options, e.g., output of reasons for the result, have a look at the options of the binaries (```--help```).

### Newer libcppl version

The API of libcppl is available from `policy-decision-point/libcppl.hh`.

```bash
LD_LIBRARY_PATH=./policy-decision-point ./policy-decision-point/cppl_compress --input-file examples/example_paper/policy_textual.cppl --policy-definition-file examples/example_paper/policy_definition.json -o examples/example_paper/policy_compressed.ccppl
LD_LIBRARY_PATH=./policy-decision-point ./policy-decision-point/cppl_evaluate --ccppl-file examples/example_paper/policy_compressed.ccppl --policy-definition-file examples/example_paper/policy_definition.json --function-handler policy-decision-point/func_handler_23.so --variables-file examples/example_paper/policy_nodeparams_1.json --runtime-variables-file examples/example_paper/policy_noderuntimeparams_1.json
```

#### Testing the python module

NOTE: You may need to adapt ```lib.linux-x86_64-2.7``` in the following commands according to your architecture.

```bash
LD_LIBRARY_PATH=policy-decision-point/ PYTHONPATH=python-module/build/lib.linux-x86_64-2.7/ python python-module/examples/example.py
LD_LIBRARY_PATH=policy-decision-point/ PYTHONPATH=python-module/build/lib.linux-x86_64-2.7/ python python-module/examples/example_memory.py
```

### Original paper version

```bash
./cppl_generator --help
```

```bash
./cppl_generator --input-file examples/example_paper/policy_textual.cppl --policy-definition-file examples/example_paper/policy_definition.json --variables-file examples/example_paper/policy_nodeparams_1.json --runtime-variables-file examples/example_paper/policy_noderuntimeparams_1.json
```

In real use cases, compression and evaluation take place on different machines. More specifically, compression takes place at the client and evaluation takes place at policy decision points, e.g., routers, load balancers, or cloud servers. The following binaries separate compression and evaluation of a policy:

```bash
./cppl_compress --input-file examples/example_paper/policy_textual.cppl --policy-definition-file examples/example_paper/policy_definition.json -o examples/example_paper/policy_compressed.ccppl
./cppl_evaluate --ccppl-file examples/example_paper/policy_compressed.ccppl --policy-definition-file examples/example_paper/policy_definition.json --variables-file examples/example_paper/policy_nodeparams_1.json --runtime-variables-file examples/example_paper/policy_noderuntimeparams_1.json
```

Furthermore, the server needs to know which parts of the policy it has to apply, i.e., which of his attributes fulfill the formula. To this end, the option ```--print-reason FILE``` of ```cppl_evaluate``` outputs these attributes as a JSON file.
```bash
./cppl_evaluate --ccppl-file examples/example_paper/policy_compressed.ccppl --policy-definition-file examples/example_paper/policy_definition.json --variables-file examples/example_paper/policy_nodeparams_1.json --runtime-variables-file examples/example_paper/policy_noderuntimeparams_1.json --print-reason examples/example_paper/evaluation_result.json
```

### Policy Decision Point

This repository contains a small example implementation for a policy decision point (PDP) and minimal examples for client and server functionality.
Note that this example PDP does not proceed beyond cppl results, i.e., a fully implemented PDP additionally needs to select an appropriate server (depending on cloud operator specific key performance indicators) and forward the request.
Similarly, the server would need to check evaluation results (reasons given as json string) and handle data according to these policy evaluation results, e.g., set up triggers for deletion at a specific point in time. As this is use case specific, it must be implemented by the systems integrating CPPL.

```
cd policy-decision-point
# Start the policy decision point
LD_LIBRARY_PATH=. ./policy_decision_point &
# Let the server register with its parameters at the policy decision point
./server
# Send an annotated query (policy decision point evaluates policy and outputs the result)
./client
# Stop policy decision point
killall policy_decision_point
```

## Debugging

The compressed policy is a binary file. To compare it with the readable debugging output it is helpful to print it as a bitstream

```bash
xxd -b examples/example_paper/policy_compressed.ccppl
```

# Research papers

The following research papers present CPPL and it's underlying concepts:

```
Martin Henze, Jens Hiller, Sascha Schmerling, Jan Henrik Ziegeldorf and Klaus Wehrle
CPPL: Compact Privacy Policy Language
Proceedings of the 15th Workshop on Privacy in the Electronic Society (WPES), co-located with the 23rd ACM Conference on Computer and Communications Security (CCS), Vienna, Austria , page 99-110.
Publisher: ACM,
October 2016
DOI: 10.1145/2994620.2994627
ISBN: 978-1-4503-4569-9
```
```
Jens Hiller, Mael Kimmerlin, Max Plauth, Seppo Heikkila, Stefan Klauck, Ville Lindfors, Felix Eberhardt, Dariusz Bursztynowski, Jesus Llorente Santos, Oliver Hohlfeld and Klaus Wehrle
Giving Customers Control over Their Data: Integrating a Policy Language into the Cloud
Proceedings of the 2018 IEEE International Conference on Cloud Engineering (IC2E 2018), Orlando, Florida, USA
Publisher: IEEE,
April 2018
accepted
```

