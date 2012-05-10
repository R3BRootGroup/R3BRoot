GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/califa/califasim.C)
add_test(califasim ${R3BROOT_BINARY_DIR}/macros/califa/califasim.sh)
SET_TESTS_PROPERTIES(califasim PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(califasim PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")
