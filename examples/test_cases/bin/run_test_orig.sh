#!/bin/bash

CPPL_COMPRESS=$1
CPPL_EVALUATE=$2

if [ ! -x $CPPL_COMPRESS ]
then
	echo "Cannot find or execute cppl_compress -> Aborting."
	exit 1
fi
if [ ! -x $CPPL_EVALUATE ]
then
	echo "Cannot find or execute cppl_evaluate -> Aborting."
	exit 1
fi

#clean files generated by last test
if [ -d generated_test_cases ]
then
	rm -rf generated_test_cases
fi

if [ -d test_result ]
then
	rm -rf test_results
fi

if [ -e policy_compressed.ccppl ]
then
	rm policy_compressed.ccppl
fi

#test cases preparation
python ./bin/test_cases_generator.py

mkdir test_results


#generate compressed policy
env LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../../policy-decision-point\
	$CPPL_COMPRESS --input-file policy_textual.cppl --policy-definition-file policy_definition.json -o policy_compressed.ccppl

#do tests
TEST_CASE_ID=0
FAILED_TESTS=0
while [ -e generated_test_cases/policy_nodeparams_$TEST_CASE_ID.json ]
do
	OUT_PUT_FILE=test_results/test_result_$TEST_CASE_ID
	echo $OUT_PUT_FILE:
	#output textual policy file
	echo -e "--------------------------------\npolicy:" > $OUT_PUT_FILE
	cat policy_textual.cppl >> $OUT_PUT_FILE

	#output node params of current test case
	echo -e "\n------------------------------\npolicy_nodeparams_$TEST_CASE_ID.json:\n" >> $OUT_PUT_FILE
	cat generated_test_cases/policy_nodeparams_$TEST_CASE_ID.json >> $OUT_PUT_FILE

	#output node runtime params of current test case
	echo -e "\n------------------------------\npolicy_noderuntimeparams_$TEST_CASE_ID.json:\n" >> $OUT_PUT_FILE
	cat generated_test_cases/policy_noderuntimeparams_$TEST_CASE_ID.json >> $OUT_PUT_FILE

	#output eval result of current test case
	echo -e "\n------------------------------\nresult:\n" >> $OUT_PUT_FILE
	env LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../../policy-decision-point\
	$CPPL_EVALUATE --ccppl-file policy_compressed.ccppl\
		--policy-definition policy_definition.json\
		--variables-file generated_test_cases/policy_nodeparams_$TEST_CASE_ID.json\
		--runtime-variables generated_test_cases/policy_noderuntimeparams_$TEST_CASE_ID.json >> $OUT_PUT_FILE

	#result from python
	echo -e "\n------------------------------\nresult from python:\c" >> $OUT_PUT_FILE
	python ./bin/test_evaluation.py generated_test_cases/policy_nodeparams_$TEST_CASE_ID.json generated_test_cases/policy_noderuntimeparams_$TEST_CASE_ID.json >> $OUT_PUT_FILE

	CHECK_SAME=$(cat $OUT_PUT_FILE | sed -n 's/\(^Policy\ result:\ \|^result\ from\ python:\)\(.*\)$/\2/p' | uniq -d)
	if [ -z "$CHECK_SAME" ]
	then
		(( FAILED_TESTS += 1 ))
		echo -e "\tTEST FAILED (results differ)" | tee -a $OUT_PUT_FILE
	else
		echo -e "\tTEST OK (obtained expected result)" | tee -a $OUT_PUT_FILE
	fi

	(( TEST_CASE_ID += 1 ))
done

if (( FAILED_TESTS == 0 ))
then
	echo "OK: All tests passed"
	exit 0
else
	echo "ERROR: $FAILED_TESTS tests failed"
	exit 1
fi
