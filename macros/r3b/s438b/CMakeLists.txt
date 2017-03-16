
file(COPY ${R3BROOT_SOURCE_DIR}/macros/r3b/s438b/sample_data_2.lmd DESTINATION ${R3BROOT_BINARY_DIR}/macros/r3b/s438b)

GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/s438b/run_sample_data.C)
add_test(landunpack ${R3BROOT_BINARY_DIR}/macros/r3b/s438b/run_sample_data.sh)
SET_TESTS_PROPERTIES(landunpack PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(landunpack PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")


