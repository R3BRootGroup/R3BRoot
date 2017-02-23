
GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/land/r3blandsim.C)
GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/land/preCalibr.C)
GENERATE_ROOT_TEST_SCRIPT(${R3BROOT_SOURCE_DIR}/macros/r3b/land/r3blandreco.C)

file(COPY ${R3BROOT_SOURCE_DIR}/macros/r3b/land/r3bcalibr.600AMeV.500keV.14m.txt DESTINATION ${R3BROOT_BINARY_DIR}/macros/r3b/land)

configure_file(${R3BROOT_SOURCE_DIR}/macros/r3b/land/run_test.sh.in ${R3BROOT_BINARY_DIR}/macros/r3b/land/run_test.sh)
add_test(landreco ${R3BROOT_BINARY_DIR}/macros/r3b/land/run_test.sh)
SET_TESTS_PROPERTIES(landreco PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES(landreco PROPERTIES PASS_REGULAR_EXPRESSION "Reconstruction successful")

add_subdirectory(test)

