
GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/run_gen_sim.C)
add_test(run_gen_sim ${R3BROOT_BINARY_DIR}/macros/r3b/run_gen_sim.sh)
SET_TESTS_PROPERTIES(run_gen_sim PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES(run_gen_sim PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")


GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/test/testR3BPhaseSpaceGeneratorIntegration.C)
add_test(testR3BPhaseSpaceGeneratorIntegration ${R3BROOT_BINARY_DIR}/macros/r3b/test/testR3BPhaseSpaceGeneratorIntegration.sh)
SET_TESTS_PROPERTIES(testR3BPhaseSpaceGeneratorIntegration PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(testR3BPhaseSpaceGeneratorIntegration PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")


GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/run_sim.C)
GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/run_digi.C)
configure_file(${R3BROOT_SOURCE_DIR}/macros/r3b/run_digi_test.sh.in ${R3BROOT_BINARY_DIR}/macros/r3b/run_digi_test.sh)
add_test(run_digi ${R3BROOT_BINARY_DIR}/macros/r3b/run_digi_test.sh)
SET_TESTS_PROPERTIES(run_digi PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES(run_digi PROPERTIES PASS_REGULAR_EXPRESSION "Digitization successful")


GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/run_aladin_sim.C)
GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/run_aladin_digi.C)
configure_file(${R3BROOT_SOURCE_DIR}/macros/r3b/run_aladin_digi_test.sh.in ${R3BROOT_BINARY_DIR}/macros/r3b/run_aladin_digi_test.sh)
add_test(run_aladin_digi ${R3BROOT_BINARY_DIR}/macros/r3b/run_aladin_digi_test.sh)
SET_TESTS_PROPERTIES(run_aladin_digi PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES(run_aladin_digi PROPERTIES PASS_REGULAR_EXPRESSION "Digitization successful")


add_subdirectory(califa)
add_subdirectory(land)
add_subdirectory(s438b)
add_subdirectory(neuland)

