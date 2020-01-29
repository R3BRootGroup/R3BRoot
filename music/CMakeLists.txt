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

# Create a library called "libR3BMusic" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES ${SYSTEM_INCLUDE_DIRECTORIES} ${BASE_INCLUDE_DIRECTORIES} )

if(EXISTS "${R3BROOT_SOURCE_DIR}/sofia/")
set(WITH_SOFIA true)
endif(EXISTS "${R3BROOT_SOURCE_DIR}/sofia/")

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/music
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/musicData
)

if(WITH_SOFIA)
set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES}
${R3BROOT_SOURCE_DIR}/sofia/sofdata/mwpcData
)
endif(WITH_SOFIA)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES ${ROOT_LIBRARY_DIR} ${FAIRROOT_LIBRARY_DIR} )

link_directories( ${LINK_DIRECTORIES})

set(SRCS
#Put here your sourcefiles
R3BMusic.cxx
R3BMusicCalPar.cxx
R3BMusicHitPar.cxx
R3BMusicContFact.cxx
R3BMusicMapped2Cal.cxx
R3BMusicCal2Hit.cxx
R3BMusicOnlineSpectra.cxx
)

if(WITH_SOFIA)
set(SRCS
${SRCS}
R3BMusicMapped2CalPar.cxx
)
endif(WITH_SOFIA)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF MusicLinkDef.h)
set(LIBRARY_NAME R3BMusic)
set(DEPENDENCIES
          R3Bbase R3BData)

if(FAIRDB_FOUND)
set(DEPENDENCIES
    ${DEPENDENCIES} FairDB)
endif(FAIRDB_FOUND)

GENERATE_LIBRARY()
