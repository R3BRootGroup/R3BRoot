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

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/tofd_v2025.6.geo.root")
message(STATUS "Starting to generate the standard root geometry v2025.6 for TofD")
execute_process(COMMAND root -l -q -x "${R3BROOT_SOURCE_DIR}/tofd/geobase/create_tofd_geo.C")
message(STATUS "${BYellow}TofD standard root geometry v2025.6 has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/tofd_v2025.6.geo.root")
message(STATUS "${BYellow}TofD standard root geometry v2025.6 already exists${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/tofd_v2025.6.geo.root")

generate_root_test_script(${R3BROOT_SOURCE_DIR}/tofd/geobase/create_tofd_geo.C)
add_test(NAME TofDRootGeometry COMMAND ${R3BROOT_BINARY_DIR}/tofd/geobase/create_tofd_geo.sh)
set_tests_properties(TofDRootGeometry PROPERTIES TIMEOUT "2000")
set_tests_properties(TofDRootGeometry PROPERTIES PASS_REGULAR_EXPRESSION
                                                  "Macro finished successfully.")
