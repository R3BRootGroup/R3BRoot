##############################################################################
#   Copyright (C) 2023 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2023 Members of R3B Collaboration                          #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/music_v2023.2.geo.root")
  message(STATUS "Starting to generate the standard root geometry for the R3B-Music")
execute_process(COMMAND root -l -q -x ${R3BROOT_SOURCE_DIR}/music/geobase/create_music_geo.C)
  message(STATUS "${BYellow}R3B-Music standard root geometry has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/music_v2023.2.geo.root")
  message(STATUS "${BYellow}R3B-Music standard root geometry already created${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/music_v2023.2.geo.root")

generate_root_test_script(${R3BROOT_SOURCE_DIR}/music/geobase/create_music_geo.C)
add_test(NAME MusicRootGeometry COMMAND ${R3BROOT_BINARY_DIR}/music/geobase/create_music_geo.sh)
set_tests_properties(MusicRootGeometry PROPERTIES TIMEOUT "2000")
set_tests_properties(MusicRootGeometry PROPERTIES PASS_REGULAR_EXPRESSION
                                                  "Macro finished successfully.")
