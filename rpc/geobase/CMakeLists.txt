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

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/tof_rpc_v22.geo.root")
message(STATUS "Starting to generate the standard root geometry for the RPC")
execute_process(COMMAND root -l -q -x ${R3BROOT_SOURCE_DIR}/rpc/geobase/create_rpc_geo.C)
message(STATUS "${BYellow}RPC standard root geometry has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/tof_rpc_v22.geo.root")
message(STATUS "${BYellow}RPC standard root geometry already created${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/tof_rpc_v22.geo.root")

enable_testing()

set(GTEST_ROOT ${SIMPATH})
find_package(GTest)

if(GTEST_FOUND)

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${SYSTEM_INCLUDE_DIRECTORIES}
    ${BASE_INCLUDE_DIRECTORIES}
    ${R3BROOT_SOURCE_DIR}/r3bbase
    ${R3BROOT_SOURCE_DIR}/rpc/geobase
    )

link_directories(${GTEST_LIBS_DIR} ${ROOT_LIBRARY_DIR} ${FAIRROOT_LIBRARY_DIR}
                 ${Boost_LIBRARY_DIRS})

set(TEST_DEPENDENCIES
    ${GTEST_BOTH_LIBRARIES}
    ${ROOT_LIBRARIES}
    FairTools
    R3BRpc
    ParBase
    GeoBase)

endif(GTEST_FOUND)

generate_root_test_script(${R3BROOT_SOURCE_DIR}/rpc/geobase/create_rpc_geo.C)
add_test(RpcRootGeometry ${R3BROOT_BINARY_DIR}/rpc/geobase/create_rpc_geo.sh)
set_tests_properties(RpcRootGeometry PROPERTIES TIMEOUT "2000")
set_tests_properties(RpcRootGeometry PROPERTIES PASS_REGULAR_EXPRESSION
                                                  "Macro finished successfully.")
