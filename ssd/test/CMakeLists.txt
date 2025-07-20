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

if(GTEST_FOUND)
    add_executable(
        SsdUnitTests
        testSsdContainerPar.cxx)
    target_link_libraries(SsdUnitTests PRIVATE GTest::gtest_main GTest::gmock_main
                                                   R3BSsd)
    gtest_discover_tests(SsdUnitTests DISCOVERY_TIMEOUT 600)
endif(GTEST_FOUND)

generate_root_test_script(${R3BROOT_SOURCE_DIR}/ssd/test/testAmsSimulation.C)
add_test(NAME AmsSimulation COMMAND ${R3BROOT_BINARY_DIR}/ssd/test/testAmsSimulation.sh)
set_tests_properties(AmsSimulation PROPERTIES TIMEOUT "2000")
set_tests_properties(AmsSimulation PROPERTIES PASS_REGULAR_EXPRESSION
                                                  "Macro finished successfully.")
