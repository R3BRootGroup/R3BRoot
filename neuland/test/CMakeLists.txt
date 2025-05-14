##############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2019-2025 Members of R3B Collaboration                     #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

if(GTest_FOUND)
    include(GoogleTest)
    add_executable(
        NeulandUnitTests
        digitizing/testNeulandDigitizingPaddle.cxx
        digitizing/testNeulandDigitizingTamex.cxx
        digitizing/testNeulandPointFilter.cxx
        NeulandUnitTests.cxx
        testClusteringEngine.cxx
        testNeulandMultiplicityCalorimetricPar.cxx)
    target_link_libraries(NeulandUnitTests PRIVATE GTest::gtest_main GTest::gmock_main
                                                   R3BNeulandDigitizing R3BNeulandReconstruction)
    gtest_discover_tests(NeulandUnitTests DISCOVERY_TIMEOUT 600)
endif(GTest_FOUND)

set(simuPars
    -o
    test_exe.simu.root
    --par-out
    test_exe.para.root
    --event-num
    100
    -s
    error
    -v
    user1)
add_test(NeulandSimulation ${R3BROOT_BINARY_DIR}/bin/neuland sim ${simuPars})
set_tests_properties(NeulandSimulation PROPERTIES TIMEOUT "2000")
set_tests_properties(NeulandSimulation PROPERTIES PASS_REGULAR_EXPRESSION
                                                  "Neuland Application finished successfully.")

set(digiPars
    -i
    test_exe.simu.root
    --par-in
    test_exe.para.root
    --paddle
    neuland
    --channel
    tamex
    -o
    test.neul.root
    -s
    info
    -v
    user1)
add_test(NeulandDigitizer ${R3BROOT_BINARY_DIR}/bin/neuland ana ${digiPars})
set_tests_properties(NeulandDigitizer PROPERTIES DEPENDS NeulandSimulation)
set_tests_properties(NeulandDigitizer PROPERTIES TIMEOUT "1000")
set_tests_properties(NeulandDigitizer PROPERTIES PASS_REGULAR_EXPRESSION
                                                 "Neuland Application finished successfully.")

generate_root_test_script(${R3BROOT_SOURCE_DIR}/neuland/test/testNeulandSimulation.C)
add_test(NeulandSimulationMacro ${R3BROOT_BINARY_DIR}/neuland/test/testNeulandSimulation.sh)

generate_root_test_script(${R3BROOT_SOURCE_DIR}/neuland/test/testNeulandDigitizer.C)
add_test(NeulandDigitizerMacro ${R3BROOT_BINARY_DIR}/neuland/test/testNeulandDigitizer.sh)
set_tests_properties(NeulandDigitizerMacro PROPERTIES DEPENDS NeulandSimulationMacro)
set_tests_properties(NeulandDigitizerMacro PROPERTIES TIMEOUT "100")

# generate_root_test_script(${R3BROOT_SOURCE_DIR}/neuland/test/testNeulandTrain.C)
# add_test(NeulandTrain ${R3BROOT_BINARY_DIR}/neuland/test/testNeulandTrain.sh)
# set_tests_properties(NeulandTrain PROPERTIES DEPENDS NeulandDigitizer)
# set_tests_properties(NeulandTrain PROPERTIES TIMEOUT "500")
# set_tests_properties(NeulandTrain PROPERTIES PASS_REGULAR_EXPRESSION "Macro finished successfully.")

# generate_root_test_script(${R3BROOT_SOURCE_DIR}/neuland/test/testNeulandReconstruction.C)
# add_test(NeulandReconstruction ${R3BROOT_BINARY_DIR}/neuland/test/testNeulandReconstruction.sh)
# set_tests_properties(NeulandReconstruction PROPERTIES DEPENDS NeulandTrain)
# set_tests_properties(NeulandReconstruction PROPERTIES TIMEOUT "500")
# set_tests_properties(NeulandReconstruction PROPERTIES PASS_REGULAR_EXPRESSION
#                                                       "Macro finished successfully.")

add_subdirectory(calibration)
