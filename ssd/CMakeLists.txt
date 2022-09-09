
##############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2019 Members of R3B Collaboration                          #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

# Create a library called "libR3BSsd" which includes the source files given
# in the array. The extension is already found. Any number of sources could be
# listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/tracking
${R3BROOT_SOURCE_DIR}/tcal
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/califaData
${R3BROOT_SOURCE_DIR}/r3bdata/trackerData
${R3BROOT_SOURCE_DIR}/r3bdata/losData
${R3BROOT_SOURCE_DIR}/r3bdata/amsData
${R3BROOT_SOURCE_DIR}/r3bdata/footData
${R3BROOT_SOURCE_DIR}/r3bdata/wrData
${R3BROOT_SOURCE_DIR}/ssd
${R3BROOT_SOURCE_DIR}/ssd/sim
${R3BROOT_SOURCE_DIR}/ssd/digi
${R3BROOT_SOURCE_DIR}/ssd/pars
${R3BROOT_SOURCE_DIR}/ssd/calibration
${R3BROOT_SOURCE_DIR}/ssd/online
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
./sim/R3BTra.cxx
./sim/R3BTraHitFinder.cxx
./pars/R3BGeoTra.cxx
./pars/R3BGeoTraPar.cxx
./pars/R3BAmsMappingPar.cxx
./pars/R3BAmsStripCalPar.cxx
./pars/R3BSsdContFact.cxx
./digi/R3BTraDigiPar.cxx
./digi/R3BTraDigitizer.cxx
./digi/R3BTargetDigiPar.cxx
./digi/R3BTargetDigitizer.cxx
./digi/R3BAmsDigitizer.cxx
./digi/R3BTra2pDigiPar.cxx
./digi/R3BTra2pDigitizer.cxx
./digi/R3BTraFraDigiPar.cxx
./digi/R3BTraFraDigitizer.cxx
./digi/R3BTarget2pDigiPar.cxx
./digi/R3BTarget2pDigitizer.cxx
./calibration/R3BAmsMapped2StripCalPar.cxx
./calibration/R3BAmsMapped2StripCal.cxx
./calibration/R3BAmsStripCal2Hit.cxx
./online/R3BAmsOnlineSpectra.cxx
./online/R3BAmsCalifaCorrelatedOnlineSpectra.cxx
./online/R3BFootOnlineSpectra.cxx
./pars/R3BFootMappingPar.cxx
./pars/R3BFootCalPar.cxx
./calibration/R3BFootMapped2StripCal.cxx
./calibration/R3BFootStripCal2Hit.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF SsdLinkDef.h)
set(LIBRARY_NAME R3BSsd)
set(DEPENDENCIES
    Spectrum R3BBase R3BData R3BTracking)

GENERATE_LIBRARY()
