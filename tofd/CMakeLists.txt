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

# Create a library called "libR3BTofD" which includes the source files given in
# the array. The extension is already found. Any number of sources could be
# listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES 
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/tofd
${R3BROOT_SOURCE_DIR}/tofd/sim
${R3BROOT_SOURCE_DIR}/tofd/pars
${R3BROOT_SOURCE_DIR}/tofd/digi
${R3BROOT_SOURCE_DIR}/tofd/calibration
${R3BROOT_SOURCE_DIR}/tofd/online
${R3BROOT_SOURCE_DIR}/tcal
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/losData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/tracking
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
./sim/R3BTofD.cxx
./digi/R3BTofDDigitizer.cxx
./digi/R3BTofDDigitizerCal.cxx
./pars/R3BTofDContFact.cxx
./pars/R3BTofDHitPar.cxx
./pars/R3BTofDHitModulePar.cxx
./pars/R3BTofDMappingPar.cxx
./pars/R3BTofDMapped2CalPar.cxx
./pars/R3BTofDCal2HitPar.cxx
./calibration/R3BTofDMapped2Cal.cxx
./calibration/R3BTofDCal2Hit.cxx
./online/R3BTofDOnlineSpectra.cxx
#R3BTofDCal2Histo.cxx
#R3BTofDCal2HistoPar.cxx
#R3BTofDChangePar.cxx
#R3BTofDCal2Hit.cxx
#R3BGeoTofD.cxx   
#R3BGeoTofDPar.cxx   
#R3BTofDContFact.cxx 
#R3BTofDDigiPar.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF TofDLinkDef.h)
set(LIBRARY_NAME R3BTofD)
set(DEPENDENCIES
    Spectrum R3BBase R3BTracking R3BData R3BTCal)
    
GENERATE_LIBRARY()
