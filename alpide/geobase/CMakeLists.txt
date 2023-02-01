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

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/target_area_alpide_twoarms_v23.geo.root")
message(STATUS "Starting to generate the standard root geometry for ALPIDE v23")
execute_process(COMMAND root -l -q -x ${R3BROOT_SOURCE_DIR}/alpide/geobase/create_target_area_2023_geo.C)
message(STATUS "${BYellow}ALPIDE v23 standard root geometry has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/target_area_alpide_twoarms_v23.geo.root")
message(STATUS "${BYellow}ALPIDE v23 standard root geometry already created${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/target_area_alpide_twoarms_v23.geo.root")

generate_root_test_script(${R3BROOT_SOURCE_DIR}/alpide/geobase/create_target_area_2023_geo.C)
add_test(AlpideRootGeometry ${R3BROOT_BINARY_DIR}/alpide/geobase/create_target_area_2023_geo.sh)
set_tests_properties(AlpideRootGeometry PROPERTIES TIMEOUT "2000")
set_tests_properties(AlpideRootGeometry PROPERTIES PASS_REGULAR_EXPRESSION
                                                  "Macro finished successfully.")
