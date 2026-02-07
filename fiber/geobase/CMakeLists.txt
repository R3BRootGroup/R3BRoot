##############################################################################
#   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2026 Members of R3B Collaboration                          #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/fi30_v2022.1.geo.root")
message(STATUS "Starting to generate the standard root geometry v2022.1 for Fiber30")
execute_process(COMMAND root -l -q -x ${R3BROOT_SOURCE_DIR}/fiber/geobase/create_fi30_geo.C)
message(STATUS "${BYellow}Fiber30 standard root geometry v2022.1 has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/fi30_v2022.1.geo.root")
message(STATUS "${BYellow}Fiber30 standard root geometry v2022.1 already exists${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/fi30_v2022.1.geo.root")

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/fi31_v2022.1.geo.root")
message(STATUS "Starting to generate the standard root geometry v2022.1 for Fiber31")
execute_process(COMMAND root -l -q -x ${R3BROOT_SOURCE_DIR}/fiber/geobase/create_fi31_geo.C)
message(STATUS "${BYellow}Fiber31 standard root geometry v2022.1 has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/fi31_v2022.1.geo.root")
message(STATUS "${BYellow}Fiber31 standard root geometry v2022.1 already exists${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/fi31_v2022.1.geo.root")

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/fi32_v2022.1.geo.root")
message(STATUS "Starting to generate the standard root geometry v2022.1 for Fiber32")
execute_process(COMMAND root -l -q -x ${R3BROOT_SOURCE_DIR}/fiber/geobase/create_fi32_geo.C)
message(STATUS "${BYellow}Fiber32 standard root geometry v2022.1 has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/fi32_v2022.1.geo.root")
message(STATUS "${BYellow}Fiber32 standard root geometry v2022.1 already exists${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/fi32_v2022.1.geo.root")

if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/fi33_v2022.1.geo.root")
message(STATUS "Starting to generate the standard root geometry v2022.1 for Fiber33")
execute_process(COMMAND root -l -q -x ${R3BROOT_SOURCE_DIR}/fiber/geobase/create_fi33_geo.C)
message(STATUS "${BYellow}Fiber33 standard root geometry v2022.1 has been created${CR} ")
else(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/fi33_v2022.1.geo.root")
message(STATUS "${BYellow}Fiber33 standard root geometry v2022.1 already exists${CR} ")
endif(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/fi33_v2022.1.geo.root")
