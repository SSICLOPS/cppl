.PHONY: test test_original test_libcppl clean
test: test_original test_libcppl

test_original:
	./bin/run_test_orig.sh ../../cppl_compress ../../cppl_evaluate

test_libcppl:
	./bin/run_test.sh ../../policy-decision-point/cppl_compress ../../policy-decision-point/cppl_evaluate

clean:
	rm -rf generated_test_cases test_results
	rm -f policy_compressed.ccppl
