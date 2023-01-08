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

# Create a library called "libR3BStartrack" which includes the source files given
# in the array. The extension is already found. Any number of sources could be 
# listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES 
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/startrack
${R3BROOT_SOURCE_DIR}/startrack/unpack
${R3BROOT_SOURCE_DIR}/startrack/unpack/B4Ucesb
${R3BROOT_SOURCE_DIR}/startrack/unpack/par
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/califaData
${R3BROOT_SOURCE_DIR}/r3bdata/startrackData
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
R3BStartrack.cxx  
R3BGeoStartrack.cxx   
R3BGeoStartrackPar.cxx   
R3BStartrackContFact.cxx  
R3BStartrackDigit.cxx
R3BStartrackEvent.cxx
unpack/R3BStartrackUnpack.cxx
unpack/R3BStartrackMapped2Cal.cxx
unpack/R3BStartrackCal2Hit.cxx
unpack/B4Ucesb/R3BStartrackRawHit.cxx
unpack/B4Ucesb/R3BStartrackRawAna.cxx
unpack/B4Ucesb/R3BStartrackRecTS.cxx
unpack/B4Ucesb/R3BStartrackOrderTS.cxx
unpack/B4Ucesb/R3BStartrackStripAna.cxx
unpack/B4Ucesb/R3BStartrackCalib.cxx
unpack/B4Ucesb/R3BStartrackCalibPar.cxx
unpack/B4Ucesb/R3BStartrackCalibParFinder.cxx
unpack/par/R3BStartrackMapped2CalPar.cxx
unpack/par/R3BStartrackHitPar.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF StartrackLinkDef.h)
set(LIBRARY_NAME R3BStartrack)
set(DEPENDENCIES
    R3BBase R3BData)

GENERATE_LIBRARY()

