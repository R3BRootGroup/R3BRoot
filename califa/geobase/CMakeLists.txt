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

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/califa_v2019.11.geo.root")
message(STATUS "Starting to generate the standard root geometry v2019.11 for CALIFA")
execute_process(COMMAND root -l -q -x "${R3BROOT_SOURCE_DIR}/califa/geobase/create_califa_geo.C(1)")
message(STATUS "${BYellow}CALIFA standard root geometry v2019.11 has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/califa_v2019.11.geo.root")
message(STATUS "${BYellow}CALIFA standard root geometry v2019.11 already exists${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/califa_v2019.11.geo.root")

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/califa_v2021.3.geo.root")
message(STATUS "Starting to generate the standard root geometry v2021.3 for CALIFA")
execute_process(COMMAND root -l -q -x "${R3BROOT_SOURCE_DIR}/califa/geobase/create_califa_geo.C(2)")
message(STATUS "${BYellow}CALIFA standard root geometry v2021.3 has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/califa_v2021.3.geo.root")
message(STATUS "${BYellow}CALIFA standard root geometry v2021.3 already exists${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/califa_v2021.3.geo.root")

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/califa_full.geo.root")
message(STATUS "Starting to generate the standard root geometry for CALIFA (FULL)")
execute_process(COMMAND root -l -q -x ${R3BROOT_SOURCE_DIR}/califa/geobase/create_califa_geo.C)
message(STATUS "${BYellow}Full CALIFA standard root geometry has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/califa_full.geo.root")
message(STATUS "${BYellow}Full CALIFA standard root geometry already exists${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/califa_full.geo.root")

enable_testing()

set(GTEST_ROOT ${SIMPATH})
find_package(GTest)

if(GTEST_FOUND)

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${SYSTEM_INCLUDE_DIRECTORIES}
    ${BASE_INCLUDE_DIRECTORIES}
    ${R3BROOT_SOURCE_DIR}/r3bbase
    ${R3BROOT_SOURCE_DIR}/califa/geobase
    )

link_directories(${GTEST_LIBS_DIR} ${ROOT_LIBRARY_DIR} ${FAIRROOT_LIBRARY_DIR}
                 ${Boost_LIBRARY_DIRS})

set(TEST_DEPENDENCIES
    ${GTEST_BOTH_LIBRARIES}
    ${ROOT_LIBRARIES}
    FairTools
    R3BCalifa
    ParBase
    GeoBase)

endif(GTEST_FOUND)

generate_root_test_script(${R3BROOT_SOURCE_DIR}/califa/geobase/create_califa_geo.C)
add_test(CalifaRootGeometry ${R3BROOT_BINARY_DIR}/califa/geobase/create_califa_geo.sh)
set_tests_properties(CalifaRootGeometry PROPERTIES TIMEOUT "2000")
set_tests_properties(CalifaRootGeometry PROPERTIES PASS_REGULAR_EXPRESSION
                                                  "Macro finished successfully.")
