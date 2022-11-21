##############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2019 Members of R3B Collaboration                          #
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

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/twinmusic_v22.geo.root")
message(STATUS "Starting to generate the standard root geometry for the Twin-Music")
execute_process(COMMAND root -l ${R3BROOT_SOURCE_DIR}/twim/geobase/create_twin_geo.C)
message(STATUS "${BYellow}Twin-Music standard root geometry has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/twinmusic_v22.geo.root")
message(STATUS "${BYellow}Twin-Music standard root geometry already created${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/twinmusic_v22.geo.root")

enable_testing()
set(PROJECT_TEST_NAME TwimUnitTests)
set(GTEST_ROOT ${SIMPATH})
find_package(GTest)

if(GTEST_FOUND)
#file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/twim/test/*.cxx)

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${SYSTEM_INCLUDE_DIRECTORIES}
    ${BASE_INCLUDE_DIRECTORIES}
    ${R3BROOT_SOURCE_DIR}/r3bbase
    ${R3BROOT_SOURCE_DIR}/r3bdata
    ${R3BROOT_SOURCE_DIR}/r3bdata/twimData
    ${R3BROOT_SOURCE_DIR}/twim/sim
    )

link_directories(${GTEST_LIBS_DIR} ${ROOT_LIBRARY_DIR} ${FAIRROOT_LIBRARY_DIR}
                 ${Boost_LIBRARY_DIRS})

set(TEST_DEPENDENCIES
    ${GTEST_BOTH_LIBRARIES}
    ${ROOT_LIBRARIES}
    FairTools
    R3BData
    ParBase
    GeoBase)

#add_executable(${PROJECT_TEST_NAME})
#target_link_libraries(${PROJECT_TEST_NAME} ${TEST_DEPENDENCIES})
add_test(${PROJECT_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_TEST_NAME})
endif(GTEST_FOUND)

generate_root_test_script(${R3BROOT_SOURCE_DIR}/twim/test/testTwimSimulation.C)
add_test(TwimSimulation ${R3BROOT_BINARY_DIR}/twim/test/testTwimSimulation.sh)
set_tests_properties(TwimSimulation PROPERTIES TIMEOUT "2000")
set_tests_properties(TwimSimulation PROPERTIES PASS_REGULAR_EXPRESSION
                                                  "Macro finished successfully.")
