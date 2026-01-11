##############################################################################
#   Copyright (C) 2020 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2020-2026 Members of R3B Collaboration                     #
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
        testR3BAtima
        testR3BAtima.cxx)
    target_link_libraries(testR3BAtima PRIVATE GTest::gtest_main GTest::gmock_main
                                                   R3BAtima)
    gtest_discover_tests(testR3BAtima DISCOVERY_TIMEOUT "100")
endif(GTEST_FOUND)
