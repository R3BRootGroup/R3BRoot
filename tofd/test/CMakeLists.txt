##############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2025 Members of R3B Collaboration                          #
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
        TofDUnitTests
        testTofDContainerPar.cxx)
    target_link_libraries(TofDUnitTests PRIVATE GTest::gtest_main GTest::gmock_main
                                                   R3BTofD)
    gtest_discover_tests(TofDUnitTests DISCOVERY_TIMEOUT "50")
endif(GTEST_FOUND)

generate_root_test_script(${R3BROOT_SOURCE_DIR}/tofd/test/testTofDSimulation.C)
add_test(NAME TofDSimulation COMMAND ${R3BROOT_BINARY_DIR}/tofd/test/testTofDSimulation.sh)
set_tests_properties(TofDSimulation PROPERTIES TIMEOUT "100")
set_tests_properties(TofDSimulation PROPERTIES PASS_REGULAR_EXPRESSION
                                                  "Macro finished successfully.")
