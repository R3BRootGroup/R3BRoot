##############################################################################
#   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
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
        FrssciUnitTests
        testFrssciContainerPar.cxx)
    target_link_libraries(FrssciUnitTests PRIVATE GTest::gtest_main GTest::gmock_main
                                                   R3BFrsSci)
    gtest_discover_tests(FrssciUnitTests DISCOVERY_TIMEOUT 600)
endif(GTEST_FOUND)
