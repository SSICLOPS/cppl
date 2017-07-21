/**
 * @file
 *  Python module for cppl functionality
 *
 */

#include <python2.7/Python.h> // ensure that this is the first include

#include "../policy-decision-point/libcppl.hh"

static PyObject *CpplError;

std::map<long long, NodeParameters *> np_list;
std::map<long long, PolicyDefinition *> pd_list;
std::map<long long, PolicyStack *> ps_list;

/*** Access to cppl library functionality ***/

/**
 * Set up stuff for cppl such as directory in /tmp/.
 */
static PyObject *cpplmodule_init(PyObject *self, PyObject *args)
{
  PyObject *retval;
  int ret;

  ret = cppl_init();
  if (ret != 0) {
    return PyErr_Format(CpplError, "cppl_init() failed");
  }

  retval = Py_BuildValue("i", ret);
  return retval;
}

/**
 * Cleanup of cppl related temporary data.
 */
static PyObject *cpplmodule_cleanup(PyObject *self, PyObject *args)
{
  PyObject *retval;
  int ret;

  ret = cppl_cleanup();
  if (ret != 0) {
    return PyErr_Format(CpplError, "cppl_cleanup() failed");
  }

  retval = Py_BuildValue("i", ret);
  return retval;
}

/**
 * Compress an uncompressed cppl policy and write the result to a file.
 */
static PyObject *cpplmodule_read_policy_from_file(PyObject *self, PyObject *args)
{
  PyObject *retval;
  char *cpplFilePath;
  string ret;

  if (!PyArg_ParseTuple(args, "s", &cpplFilePath)) {
    return NULL;
  }

  try {
    ret = cppl_read_policy_from_file(cpplFilePath);
  }
  catch (std::string s) {
    return PyErr_Format(CpplError, "%s", s.c_str());
  }

  retval = Py_BuildValue("s", ret.c_str());
  return retval;
}

/**
 * Compress an uncompressed cppl policy and write the result to a file.
 */
static PyObject *cpplmodule_compress_policy_to_file(PyObject *self, PyObject *args)
{
  PyObject *retval;
  long long ret, pd_key;
  bool ret_bool;
  char *policy, *outputFile;
  PolicyDefinition *pd;

  if (!PyArg_ParseTuple(args, "sLs", &policy, &pd_key, &outputFile)) {
    return NULL;
  }

  bool traceParsingEnabled = false;
  bool traceScanningEnabled = false;

  pd = pd_list.at(pd_key);

  try {
    ret_bool = cppl_compress_policy_to_file(policy, *pd, outputFile, traceParsingEnabled, traceScanningEnabled);
  }
  catch (std::runtime_error e) {
    return PyErr_Format(CpplError, "%s", e.what());
  }

  if( ret_bool ) {
    ret = 1;
  } else {
    ret = 0;
  }

  retval = Py_BuildValue("i", ret);
  return retval;
}

/**
 * Compress an uncompressed cppl policy.
 */
static PyObject *cpplmodule_compress_policy(PyObject *self, PyObject *args)
{
  PyObject *retval;
  long long pd_key;
  char *policy, *compressed_policy_mem;
  int compressed_policy_mem_len;
  PolicyDefinition *pd;
  Binary *compressed_policy;

  if (!PyArg_ParseTuple(args, "sL", &policy, &pd_key)) {
    return NULL;
  }

  bool traceParsingEnabled = false;
  bool traceScanningEnabled = false;

  pd = pd_list.at(pd_key);

  try {
    compressed_policy = cppl_compress_policy(policy, *pd, traceParsingEnabled, traceScanningEnabled);
  }
  catch (std::runtime_error e) {
    return PyErr_Format(CpplError, "%s", e.what());
  }

  if(!compressed_policy) {
    return PyErr_Format(CpplError, "Compression failed");
  }

  compressed_policy_mem = (char *)compressed_policy->getPointer();
  compressed_policy_mem_len = ceil((double)compressed_policy->size() / 8);

  retval = Py_BuildValue("s#", compressed_policy_mem, compressed_policy_mem_len);
  delete compressed_policy;
  return retval;
}

/**
 * Read policy definition and corresponding function handler library from disk.
 */
static PyObject *cpplmodule_read_policy_definition_from_file(PyObject *self, PyObject *args)
{
  PyObject *retval;
  char *policyDefinitionFile, *functionHandlerLibPath;
  PolicyDefinition *pd;

  if (!PyArg_ParseTuple(args, "ss", &policyDefinitionFile, &functionHandlerLibPath)) {
    return NULL;
  }

  try {
    pd = cppl_read_policy_definition_from_file(policyDefinitionFile, functionHandlerLibPath);
  }
  catch (std::string s) {
    return PyErr_Format(CpplError, "%s", s.c_str());
  }

  long long pd_key = (long long)pd;
  pd_list[pd_key] = pd;

  retval = Py_BuildValue("L", pd_key);
  return retval;
}

/**
 * Get policy definition and corresponding function handler from memory.
 */
static PyObject *cpplmodule_read_policy_definition(PyObject *self, PyObject *args)
{
  PyObject *retval;
  char *policyDefinitionString, *functionHandlerLibBuf;
  int functionHandlerLibBuf_len;
  PolicyDefinition *pd;
  Binary functionHandlerLib;

  if (!PyArg_ParseTuple(args, "ss#", &policyDefinitionString, &functionHandlerLibBuf, &functionHandlerLibBuf_len)) {
    return NULL;
  }

  functionHandlerLib.read_from_mem(functionHandlerLibBuf, functionHandlerLibBuf_len * 8);

  try {
    pd = cppl_read_policy_definition(policyDefinitionString, &functionHandlerLib);
  }
  catch (std::string s) {
    return PyErr_Format(CpplError, "%s", s.c_str());
  }

  long long pd_key = (long long)pd;
  pd_list[pd_key] = pd;

  retval = Py_BuildValue("L", pd_key);
  return retval;
}


/**
 * Get node parameters from disk.
 */
static PyObject *cpplmodule_read_node_parameters_from_file(PyObject *self, PyObject *args)
{
  PyObject *retval;
  char *nodeParamsPath, *nodeRuntimeParamsPath;
  long long pd_key;

  PolicyDefinition *pd;
  NodeParameters *np;

  if (!PyArg_ParseTuple(args, "ssL", &nodeParamsPath, &nodeRuntimeParamsPath, &pd_key)) {
    return NULL;
  }

  pd = pd_list.at(pd_key);

  try {
    np = cppl_read_node_parameters_from_file(nodeParamsPath, nodeRuntimeParamsPath, pd);
  }
  catch (std::string s) {
    return PyErr_Format(CpplError, "%s", s.c_str());
  }

  long long np_key = (long long)np;
  np_list[np_key] = np;

  retval = Py_BuildValue("L", np_key);
  return retval;
}

/**
 * Get node parameters from memory.
 */
static PyObject *cpplmodule_read_node_parameters(PyObject *self, PyObject *args)
{
  PyObject *retval;
  char *nodeParamsString, *nodeRuntimeParamsString;
  long long pd_key;

  PolicyDefinition *pd;
  NodeParameters *np;

  if (!PyArg_ParseTuple(args, "ssL", &nodeParamsString, &nodeRuntimeParamsString, &pd_key)) {
    return NULL;
  }

  pd = pd_list.at(pd_key);

  try {
    np = cppl_read_node_parameters(nodeParamsString, nodeRuntimeParamsString, pd);
  }
  catch (std::string s) {
    return PyErr_Format(CpplError, "%s", s.c_str());
  }

  long long np_key = (long long)np;
  np_list[np_key] = np;

  retval = Py_BuildValue("L", np_key);
  return retval;
}

/**
 * Get compressed cppl policy from file.
 */
static PyObject *cpplmodule_read_compressed_cppl_from_file(PyObject *self, PyObject *args)
{
  PyObject *retval;
  char *ccpplFilePath;
  long long pd_key;
  PolicyDefinition *pd;
  PolicyStack *compressed_policy;

  if (!PyArg_ParseTuple(args, "sL", &ccpplFilePath, &pd_key)) {
    return NULL;
  }

  pd = pd_list.at(pd_key);

  try {
    compressed_policy = cppl_read_compressed_cppl_from_file(ccpplFilePath, pd);
  }
  catch (std::runtime_error e) {
    return PyErr_Format(CpplError, "%s", e.what());
  }
  catch (std::string s) {
    return PyErr_Format(CpplError, "%s", s.c_str());
  }

  long long ps_key = (long long)compressed_policy;
  ps_list[ps_key] = compressed_policy;

  retval = Py_BuildValue("L", ps_key);
  return retval;
}

/**
 * Get compressed cppl policy from memory.
 */
static PyObject *cpplmodule_read_compressed_cppl(PyObject *self, PyObject *args)
{
  PyObject *retval;
  char *ccpplDataBuf;
  int ccpplDataBuf_len;
  long long pd_key;
  PolicyDefinition *pd;
  PolicyStack *compressed_policy;
  Binary ccpplData;

  if (!PyArg_ParseTuple(args, "s#L", &ccpplDataBuf, &ccpplDataBuf_len, &pd_key)) {
    return NULL;
  }

  ccpplData.read_from_mem(ccpplDataBuf, ccpplDataBuf_len * 8);

  pd = pd_list.at(pd_key);

  try {
    compressed_policy = cppl_read_compressed_cppl(ccpplData, pd);
  }
  catch (std::runtime_error e) {
    return PyErr_Format(CpplError, "%s", e.what());
  }
  catch (std::string s) {
    return PyErr_Format(CpplError, "%s", s.c_str());
  }

  long long ps_key = (long long)compressed_policy;
  ps_list[ps_key] = compressed_policy;

  retval = Py_BuildValue("L", ps_key);
  return retval;
}


/**
 * @return True of the nodeParameters fulfill the policy, False otherwise.
 */
static PyObject *cpplmodule_evaluate(PyObject *self, PyObject *args)
{
  PyObject *retval;
  bool ret_bool;
  long long np_key, ps_key;
  int ret;
  PolicyStack *compressed_policy;
  NodeParameters *np;

  if (!PyArg_ParseTuple(args, "iL", &ps_key, &np_key)) {
    return NULL;
  }

  compressed_policy = ps_list.at(ps_key);
  np = np_list.at(np_key);

  ret_bool = cppl_evaluate(compressed_policy, np);

  if( ret_bool ) {
    ret = 1;
  } else {
    ret = 0;
  }

  retval = Py_BuildValue("i", ret);
  return retval;
}

/**
 * Get a string that describes the reason of the policy results. Only use after cppl_evaluate.
 *
 * Note that if the policy is not fulfilled, cppl_get_reason will require significantly
 * more time compared to a fulfilled policy. Thus, checking unfulfilled policies for
 * a reason can significantly can result in significant performance drops.
 */
static PyObject *cpplmodule_get_reason(PyObject *self, PyObject *args)
{
  PyObject *retval;
  long long ps_key, pd_key;
  string ret;
  PolicyStack *compressed_policy;
  PolicyDefinition *pd;

  if (!PyArg_ParseTuple(args, "LL", &ps_key, &pd_key)) {
    return NULL;
  }

  compressed_policy = ps_list.at(ps_key);
  pd = pd_list.at(pd_key);

  ret = cppl_get_reason(compressed_policy, pd);

  retval = Py_BuildValue("s", ret.c_str());
  return retval;
}

/*** Memory management functions ***/

static PyObject *cpplmodule_policy_definition_free(PyObject *self, PyObject *args)
{
  long long pd_key;
  PolicyDefinition *policy_definition;

  if (!PyArg_ParseTuple(args, "L", &pd_key)) {
    return NULL;
  }

  policy_definition = pd_list.at(pd_key);

  pd_list.erase(pd_key);
  delete policy_definition;
  policy_definition = NULL;

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *cpplmodule_node_parameters_free(PyObject *self, PyObject *args)
{
  long long np_key;
  NodeParameters *node_parameters;

  if (!PyArg_ParseTuple(args, "L", &np_key)) {
    return NULL;
  }

  node_parameters = np_list.at(np_key);

  np_list.erase(np_key);
  delete node_parameters;
  node_parameters = NULL;

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *cpplmodule_compressed_cppl_free(PyObject *self, PyObject *args)
{
  long long ps_key;
  PolicyStack *compressed_policy;

  if (!PyArg_ParseTuple(args, "L", &ps_key)) {
    return NULL;
  }

  compressed_policy = ps_list.at(ps_key);

  ps_list.erase(ps_key);
  delete compressed_policy;
  compressed_policy = NULL;

  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef cpplMethods[] = {
    {
        "init",
        cpplmodule_init,
        METH_NOARGS,
        "Initialize the CPPL library."
    },
    {
        "cleanup",
        cpplmodule_cleanup,
        METH_NOARGS,
        "Uninitialize the CPPL library."
    },
    {
        "compress_policy",
        cpplmodule_compress_policy,
        METH_VARARGS,
        "Compress policy."
    },
    {
        "compress_policy_to_file",
        cpplmodule_compress_policy_to_file,
        METH_VARARGS,
        "Compress policy storing the result on disk."
    },
    {
        "read_policy_from_file",
        cpplmodule_read_policy_from_file,
        METH_VARARGS,
        "Read the content of a cppl file."
    },
    {
        "read_policy_definition_from_file",
        cpplmodule_read_policy_definition_from_file,
        METH_VARARGS,
        "Read the content of a cppl definition file."
    },
    {
        "read_policy_definition",
        cpplmodule_read_policy_definition,
        METH_VARARGS,
        "Read the content of a policy definition and the corresponding function handler file"
    },
    {
        "read_node_parameters_from_file",
        cpplmodule_read_node_parameters_from_file,
        METH_VARARGS,
        "Read node parameters and node runtime parameters from disk"
    },
    {
        "read_node_parameters",
        cpplmodule_read_node_parameters,
        METH_VARARGS,
        "Read node parameters and node runtime parameters from memory"
    },
    {
        "read_compressed_cppl_from_file",
        cpplmodule_read_compressed_cppl_from_file,
        METH_VARARGS,
        "Read compressed policy from file"
    },
    {
        "read_compressed_cppl",
        cpplmodule_read_compressed_cppl,
        METH_VARARGS,
        "Read compressed policy from memory"
    },
    {
        "evaluate",
        cpplmodule_evaluate,
        METH_VARARGS,
        "TODO"
    },
    {
        "get_reason",
        cpplmodule_get_reason,
        METH_VARARGS,
        "Get more detailed information on the evaluation result"
    },
    {
        "policy_definition_free",
        cpplmodule_policy_definition_free,
        METH_VARARGS,
        "Deallocate memory of PolicyDefinition instance"
    },
    {
        "node_parameters_free",
        cpplmodule_node_parameters_free,
        METH_VARARGS,
        "Deallocate memory of NodeParameters instance"
    },
    {
        "compressed_cppl_free",
        cpplmodule_compressed_cppl_free,
        METH_VARARGS,
       "Deallocate memory of PolicyStack instance"
    },
    {
        NULL,
        NULL,
        0,
        NULL
    }   /* Sentinel */
};

PyMODINIT_FUNC initcppl_cpp_python_bridge(void)
{
  PyObject *m;

  m = Py_InitModule("cppl_cpp_python_bridge", cpplMethods);
  if (m == NULL) {
    return;
  }

  CpplError = PyErr_NewException("cppl.error", NULL, NULL);
  Py_INCREF(CpplError);
  PyModule_AddObject(m, "error", CpplError);
}

