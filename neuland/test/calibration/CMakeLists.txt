generate_root_test_script(${R3BROOT_SOURCE_DIR}/neuland/test/calibration/testNeulandQcal.C)
add_test(NeulandQcal ${R3BROOT_BINARY_DIR}/neuland/test/calibration/testNeulandQcal.sh)
set_tests_properties(NeulandQcal PROPERTIES TIMEOUT "100")
set_tests_properties(NeulandQcal PROPERTIES PASS_REGULAR_EXPRESSION "Test successful!")

generate_root_test_script(${R3BROOT_SOURCE_DIR}/neuland/test/calibration/testNeulandTcal.C)
add_test(NeulandTcal ${R3BROOT_BINARY_DIR}/neuland/test/calibration/testNeulandTcal.sh)
set_tests_properties(NeulandTcal PROPERTIES TIMEOUT "100")
set_tests_properties(NeulandTcal PROPERTIES PASS_REGULAR_EXPRESSION "Test successful!")

#generate_root_test_script(${R3BROOT_SOURCE_DIR}/neuland/test/calibration/testNeulandCosmic1.C)
#add_test(NeulandCosmic1 ${R3BROOT_BINARY_DIR}/neuland/test/calibration/testNeulandCosmic1.sh)
#set_tests_properties(NeulandCosmic1 PROPERTIES TIMEOUT "3600")
#set_tests_properties(NeulandCosmic1 PROPERTIES PASS_REGULAR_EXPRESSION "Test successful!")
