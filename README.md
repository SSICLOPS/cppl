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
