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

# Create a library called "libR3BTwim" which includes the source files given in the
# array. The extension is already found. Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES ${SYSTEM_INCLUDE_DIRECTORIES} ${BASE_INCLUDE_DIRECTORIES})

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/tracking
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/twimData
${R3BROOT_SOURCE_DIR}/r3bdata/mwpcData
${R3BROOT_SOURCE_DIR}/twim
${R3BROOT_SOURCE_DIR}/twim/digi
${R3BROOT_SOURCE_DIR}/twim/sim
${R3BROOT_SOURCE_DIR}/twim/pars
${R3BROOT_SOURCE_DIR}/twim/calibration
${R3BROOT_SOURCE_DIR}/twim/online
${R3BROOT_SOURCE_DIR}/twim/ana
)

if(EXISTS "${R3BROOT_SOURCE_DIR}/sofia/")
set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES}
${R3BROOT_SOURCE_DIR}/sofia/sofdata
${R3BROOT_SOURCE_DIR}/sofia/sofdata/tofwData
)
endif(EXISTS "${R3BROOT_SOURCE_DIR}/sofia/")

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES ${ROOT_LIBRARY_DIR} ${FAIRROOT_LIBRARY_DIR} )

link_directories( ${LINK_DIRECTORIES})

#set(SRCS
#Put here your sourcefiles
#*.cxx
#./sim/R3BSofTwim.cxx
#./sim/R3BSofTwimDigitizer.cxx
#./par/R3BSofTwimContFact.cxx
#./par/R3BSofTwimCalPar.cxx
#./par/R3BSofTwimHitPar.cxx
#./calibration/R3BSofTwimMapped2Cal.cxx
#./calibration/R3BSofTwimCal2Hit.cxx
#./calibration/R3BSofTwimMapped2CalPar.cxx
#./ana/R3BTwimVertexReconstruction.cxx
#)

file(GLOB SRCS 
./sim/*.cxx 
./digi/*.cxx
./pars/*.cxx 
./online/*.cxx
./calibration/*.cxx
./ana/*.cxx
)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF TwimLinkDef.h)
set(LIBRARY_NAME R3BTwim)
set(DEPENDENCIES
  R3BBase R3BData R3BTracking)

GENERATE_LIBRARY()
