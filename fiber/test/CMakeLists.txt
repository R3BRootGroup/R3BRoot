##############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2022-2026 Members of R3B Collaboration                     #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

if(GTEST_FOUND)
    add_executable(
        FiberUnitTests
        testFiberContainerPar.cxx)
    target_link_libraries(FiberUnitTests PRIVATE GTest::gtest_main GTest::gmock_main
                                                   R3BBunchedFiber)
    gtest_discover_tests(FiberUnitTests DISCOVERY_TIMEOUT "50")
endif(GTEST_FOUND)

#generate_root_test_script(${R3BROOT_SOURCE_DIR}/fiber/test/testFiberSimulation.C)
#add_test(NAME FiberSimulation COMMAND ${R3BROOT_BINARY_DIR}/fiber/test/testFiberSimulation.sh)
#set_tests_properties(FiberSimulation PROPERTIES TIMEOUT "100")
#set_tests_properties(FiberSimulation PROPERTIES PASS_REGULAR_EXPRESSION
#                                                  "Macro finished successfully.")
