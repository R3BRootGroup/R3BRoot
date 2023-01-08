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

# Create a library called "R3BBunchedFiber" which includes the source files
# given in the array. The extension is already found. Any number of 
# sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES 
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/fibData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/fiber
${R3BROOT_SOURCE_DIR}/fiber/online
${R3BROOT_SOURCE_DIR}/fiber/sim
${R3BROOT_SOURCE_DIR}/fiber/digi
${R3BROOT_SOURCE_DIR}/fiber/pars
${R3BROOT_SOURCE_DIR}/fiber/mapmt
${R3BROOT_SOURCE_DIR}/fiber/bunched
${R3BROOT_SOURCE_DIR}/tcal
${R3BROOT_SOURCE_DIR}/tracking
)

include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories(${LINK_DIRECTORIES})

set(SRCS
./sim/R3BFiber.cxx
./digi/R3BFiberDigitizer.cxx
./bunched/R3BBunchedFiberMapped2CalPar.cxx
./bunched/R3BBunchedFiberMapped2Cal.cxx
./bunched/R3BBunchedFiberCal2Hit.cxx
./bunched/R3BBunchedFiberCal2Hit_s494.cxx
./bunched/R3BBunchedFiberSPMTTrigMapped2CalPar.cxx
./bunched/R3BBunchedFiberSPMTTrigMapped2Cal.cxx
./mapmt/R3BFiberMAPMTMapped2Cal.cxx
./mapmt/R3BFiberMAPMTCal2Hit.cxx
./mapmt/R3BFiberMAPMTMapped2CalPar.cxx
./pars/R3BFiberContFact.cxx
./pars/R3BBunchedFiberHitModulePar.cxx
./pars/R3BBunchedFiberHitPar.cxx
./pars/R3BFiberMAPMTHitPar.cxx
./pars/R3BFiberMAPMTHitModulePar.cxx
./pars/R3BFiberMappingPar.cxx
./online/R3BFiberMAPMTOnlineSpectra.cxx
./online/R3BFiberMAPMTCorrelationOnlineSpectra.cxx
)

CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF FiberLinkDef.h)

Set(LIBRARY_NAME R3BBunchedFiber)

set(DEPENDENCIES
R3BData R3BTCal R3BTracking R3BBase
)

GENERATE_LIBRARY()
