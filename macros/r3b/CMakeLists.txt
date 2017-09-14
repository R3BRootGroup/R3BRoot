
GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/r3bsim.C)
add_test(r3bsim ${R3BROOT_BINARY_DIR}/macros/r3b/r3bsim.sh)
SET_TESTS_PROPERTIES(r3bsim PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(r3bsim PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")


GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/r3bsim_gen.C)
add_test(r3bsim_gen ${R3BROOT_BINARY_DIR}/macros/r3b/r3bsim_gen.sh)
SET_TESTS_PROPERTIES(r3bsim_gen PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(r3bsim_gen PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")


GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/r3bsim_g4.C)
add_test(r3bsim_g4 ${R3BROOT_BINARY_DIR}/macros/r3b/r3bsim_g4.sh)
SET_TESTS_PROPERTIES(r3bsim_g4 PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(r3bsim_g4 PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")


GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/r3bsim_new.C)
add_test(r3bsim_new ${R3BROOT_BINARY_DIR}/macros/r3b/r3bsim_new.sh)
SET_TESTS_PROPERTIES(r3bsim_new PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(r3bsim_new PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")


GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/run_sim.C)
GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/run_digi.C)
configure_file(${R3BROOT_SOURCE_DIR}/macros/r3b/run_digi_test.sh.in ${R3BROOT_BINARY_DIR}/macros/r3b/run_digi_test.sh)
add_test(run_digi ${R3BROOT_BINARY_DIR}/macros/r3b/run_digi_test.sh)
SET_TESTS_PROPERTIES(run_digi PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(run_digi PROPERTIES PASS_REGULAR_EXPRESSION "Digitization successful")


GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/run_aladin_sim.C)
GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/run_aladin_digi.C)
configure_file(${R3BROOT_SOURCE_DIR}/macros/r3b/run_aladin_digi_test.sh.in ${R3BROOT_BINARY_DIR}/macros/r3b/run_aladin_digi_test.sh)
add_test(run_aladin_digi ${R3BROOT_BINARY_DIR}/macros/r3b/run_aladin_digi_test.sh)
SET_TESTS_PROPERTIES(run_aladin_digi PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(run_aladin_digi PROPERTIES PASS_REGULAR_EXPRESSION "Digitization successful")


add_subdirectory(califa)
add_subdirectory(land)
add_subdirectory(s438b)
add_subdirectory(neuland)

