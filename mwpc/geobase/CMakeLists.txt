##############################################################################
#   Copyright (C) 2021 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2021-2023 Members of R3B Collaboration                     #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc0_v2021.3.geo.root")
message(STATUS "Starting to generate the standard root geometry for the MWPC0")
execute_process(COMMAND root -l -q -x ${R3BROOT_SOURCE_DIR}/mwpc/geobase/create_mwpc0_geo.C)
message(STATUS "${BYellow}MWPC0 standard root geometry has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc0_v2021.3.geo.root")
message(STATUS "${BYellow}MWPC0 standard root geometry already created${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc0_v2021.3.geo.root")

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc1_v2021.3.geo.root")
message(STATUS "Starting to generate the standard root geometry for the MWPC1")
execute_process(COMMAND root -l -q -x "${R3BROOT_SOURCE_DIR}/mwpc/geobase/create_mwpc1and2_geo.C(1)")
message(STATUS "${BYellow}MWPC1 standard root geometry has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc1_v2021.3.geo.root")
message(STATUS "${BYellow}MWPC1 standard root geometry already created${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc1_v2021.3.geo.root")

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc2_v2021.3.geo.root")
message(STATUS "Starting to generate the standard root geometry for the MWPC2")
execute_process(COMMAND root -l -q -x "${R3BROOT_SOURCE_DIR}/mwpc/geobase/create_mwpc1and2_geo.C(2)")
message(STATUS "${BYellow}MWPC2 standard root geometry has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc2_v2021.3.geo.root")
message(STATUS "${BYellow}MWPC2 standard root geometry already created${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc2_v2021.3.geo.root")

if(WITH_SOFIA)
if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc3_v2021.3.geo.root")
message(STATUS "Starting to generate the standard root geometry for the MWPC3")
execute_process(COMMAND root -l -q -x ${R3BROOT_SOURCE_DIR}/mwpc/geobase/create_mwpc3_geo.C)
message(STATUS "${BYellow}MWPC3 standard root geometry has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc3_v2021.3.geo.root")
message(STATUS "${BYellow}MWPC3 standard root geometry already created${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/mwpc3_v2021.3.geo.root")
endif(WITH_SOFIA)
