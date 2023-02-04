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

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc_0.geo.root")
message(STATUS "Starting to generate the standard root geometry for the MWPC0")
execute_process(COMMAND root -l -q -x ${R3BROOT_SOURCE_DIR}/mwpc/geobase/create_mwpc0_geo.C)
message(STATUS "${BYellow}MWPC0 standard root geometry has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc_0.geo.root")
message(STATUS "${BYellow}MWPC0 standard root geometry already created${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc_0.geo.root")

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc_1.geo.root")
message(STATUS "Starting to generate the standard root geometry for the MWPC1")
execute_process(COMMAND root -l -q -x "${R3BROOT_SOURCE_DIR}/mwpc/geobase/create_mwpc1and2_geo.C(1)")
message(STATUS "${BYellow}MWPC1 standard root geometry has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc_1.geo.root")
message(STATUS "${BYellow}MWPC1 standard root geometry already created${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc_1.geo.root")

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc_2.geo.root")
message(STATUS "Starting to generate the standard root geometry for the MWPC2")
execute_process(COMMAND root -l -q -x "${R3BROOT_SOURCE_DIR}/mwpc/geobase/create_mwpc1and2_geo.C(2)")
message(STATUS "${BYellow}MWPC2 standard root geometry has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc_2.geo.root")
message(STATUS "${BYellow}MWPC2 standard root geometry already created${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc_2.geo.root")

if(WITH_SOFIA)
if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc_3.geo.root")
message(STATUS "Starting to generate the standard root geometry for the MWPC3")
execute_process(COMMAND root -l -q -x ${R3BROOT_SOURCE_DIR}/mwpc/geobase/create_mwpc3_geo.C)
message(STATUS "${BYellow}MWPC3 standard root geometry has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc_3.geo.root")
message(STATUS "${BYellow}MWPC3 standard root geometry already created${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc_3.geo.root")
endif(WITH_SOFIA)

generate_root_test_script(${R3BROOT_SOURCE_DIR}/mwpc/geobase/create_mwpc0_geo.C)
add_test(NAME Mwpc0RootGeometry COMMAND ${R3BROOT_BINARY_DIR}/mwpc/geobase/create_mwpc0_geo.sh)
set_tests_properties(Mwpc0RootGeometry PROPERTIES TIMEOUT "2000")
set_tests_properties(Mwpc0RootGeometry PROPERTIES PASS_REGULAR_EXPRESSION
                                                  "Macro finished successfully.")
                                                                                                    
generate_root_test_script(${R3BROOT_SOURCE_DIR}/mwpc/geobase/create_mwpc1and2_geo.C)
add_test(NAME Mwpc1RootGeometry COMMAND ${R3BROOT_BINARY_DIR}/mwpc/geobase/create_mwpc1and2_geo.sh)
set_tests_properties(Mwpc1RootGeometry PROPERTIES TIMEOUT "2000")
set_tests_properties(Mwpc1RootGeometry PROPERTIES PASS_REGULAR_EXPRESSION
                                                  "Macro finished successfully.")
                                                  
if(WITH_SOFIA)                                              
generate_root_test_script(${R3BROOT_SOURCE_DIR}/mwpc/geobase/create_mwpc3_geo.C)
add_test(NAME Mwpc3RootGeometry COMMAND ${R3BROOT_BINARY_DIR}/mwpc/geobase/create_mwpc3_geo.sh)
set_tests_properties(Mwpc3RootGeometry PROPERTIES TIMEOUT "2000")
set_tests_properties(Mwpc3RootGeometry PROPERTIES PASS_REGULAR_EXPRESSION
                                                  "Macro finished successfully.")
endif(WITH_SOFIA)
