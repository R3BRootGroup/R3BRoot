GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/land/test/Land_qcal_test.C)
GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/land/test/Land_tcal_test.C)
GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/land/test/Land_cosmic1_test.C)

add_test(land_qcal ${R3BROOT_BINARY_DIR}/macros/r3b/land/test/Land_qcal_test.sh)
SET_TESTS_PROPERTIES(land_qcal PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(land_qcal PROPERTIES PASS_REGULAR_EXPRESSION "Test successful!")

add_test(land_tcal ${R3BROOT_BINARY_DIR}/macros/r3b/land/test/Land_tcal_test.sh)
SET_TESTS_PROPERTIES(land_tcal PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(land_tcal PROPERTIES PASS_REGULAR_EXPRESSION "Test successful!")

add_test(land_cosmic1 ${R3BROOT_BINARY_DIR}/macros/r3b/land/test/Land_cosmic1_test.sh)
SET_TESTS_PROPERTIES(land_cosmic1 PROPERTIES TIMEOUT "3600")
SET_TESTS_PROPERTIES(land_cosmic1 PROPERTIES PASS_REGULAR_EXPRESSION "Test successful!")
