##############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2019-2023 Members of R3B Collaboration                     #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

cmake_minimum_required(VERSION 3.0)

enable_testing()
set(PROJECT_TEST_NAME MusicUnitTests)
set(GTEST_ROOT ${SIMPATH})
find_package(GTest)

if(GTEST_FOUND)
file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/music/test/*.cxx)

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${SYSTEM_INCLUDE_DIRECTORIES}
    ${BASE_INCLUDE_DIRECTORIES}
    ${R3BROOT_SOURCE_DIR}/r3bbase
    ${R3BROOT_SOURCE_DIR}/r3bdata
    ${R3BROOT_SOURCE_DIR}/r3bdata/musicData
    ${R3BROOT_SOURCE_DIR}/music/sim
    ${R3BROOT_SOURCE_DIR}/music/pars
    ${R3BROOT_SOURCE_DIR}/music/digi
    )

link_directories(${GTEST_LIBS_DIR} ${ROOT_LIBRARY_DIR} ${FAIRROOT_LIBRARY_DIR}
                 ${Boost_LIBRARY_DIRS})

set(TEST_DEPENDENCIES
    ${GTEST_BOTH_LIBRARIES}
    ${ROOT_LIBRARIES}
    FairTools
    R3BData
    R3BMusic
    ParBase
    GeoBase)

add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
target_link_libraries(${PROJECT_TEST_NAME} ${TEST_DEPENDENCIES})
add_test(${PROJECT_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_TEST_NAME})
endif(GTEST_FOUND)

generate_root_test_script(${R3BROOT_SOURCE_DIR}/music/test/testMusicSimulation.C)
add_test(MusicSimulation ${R3BROOT_BINARY_DIR}/music/test/testMusicSimulation.sh)
set_tests_properties(MusicSimulation PROPERTIES TIMEOUT "2000")
set_tests_properties(MusicSimulation PROPERTIES PASS_REGULAR_EXPRESSION
                                                  "Macro finished successfully.")
