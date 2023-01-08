
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

# Create a library called "libR3BTofi" which includes the source files given
# in the array. The extension is already found. Any number of sources could be 
# listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES 
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/losData
${R3BROOT_SOURCE_DIR}/tcal
${R3BROOT_SOURCE_DIR}/tracking
${R3BROOT_SOURCE_DIR}/tofi/pars
${R3BROOT_SOURCE_DIR}/tofi/sim
${R3BROOT_SOURCE_DIR}/tofi/digi
${R3BROOT_SOURCE_DIR}/tofi/calibration
${R3BROOT_SOURCE_DIR}/tofi
${R3BROOT_SOURCE_DIR}/r3bdata/tofiData
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
#Put here your sourcefiles
./sim/R3BTofi.cxx
./sim/R3BGeoTofi.cxx
./pars/R3BGeoTofiPar.cxx
./pars/R3BTofiDigiPar.cxx
./digi/R3BTofiDigitizerCal.cxx
./pars/R3BTofiHisto2HitPar.cxx
./pars/R3BTofiHitPar.cxx
./pars/R3BTofiHitModulePar.cxx
./pars/R3BTofiChangePar.cxx
./pars/R3BTofiContFact.cxx
./calibration/R3BTofiCal2Histo.cxx
./calibration/R3BTofiMapped2Cal.cxx
./calibration/R3BTofiMapped2CalPar.cxx
./calibration/R3BTofiCal2HitS494.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF TofiLinkDef.h)
set(LIBRARY_NAME R3BTofi)
set(DEPENDENCIES
    R3BBase R3BData R3BTCal R3BTracking)

GENERATE_LIBRARY()

