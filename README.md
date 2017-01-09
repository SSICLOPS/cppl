# CPPL - Compact Privacy Policy Language

## Build Instructions

### Preliminaries

Tested with Ubuntu 14.04

```bash
sudo apt-get install g++ flex bison libboost-all-dev libjsoncpp-dev
```

### Configuration

See `debug.hh` and `options.hh`, e.g., ```#define DEBUG_POLICY_GENERATION 1``` enables output of the policy generation including the compressed policy.

### Compiling

```bash
make
```

## Usage Examples

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

## Debugging

The compressed policy is a binary file. To compare it with the readable debugging output it is helpful to print it as a bitstream

```bash
xxd -b examples/example_paper/policy_compressed.ccppl
```
