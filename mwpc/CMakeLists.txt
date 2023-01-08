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

# Create a library called "libR3BMwpc" which includes the source files given in the
# array. The extension is already found. Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES ${SYSTEM_INCLUDE_DIRECTORIES} ${BASE_INCLUDE_DIRECTORIES})

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/tracking
${R3BROOT_SOURCE_DIR}/mwpc
${R3BROOT_SOURCE_DIR}/mwpc/digi
${R3BROOT_SOURCE_DIR}/mwpc/online
${R3BROOT_SOURCE_DIR}/mwpc/mwpc0
${R3BROOT_SOURCE_DIR}/mwpc/mwpc1
${R3BROOT_SOURCE_DIR}/mwpc/mwpc2
${R3BROOT_SOURCE_DIR}/mwpc/mwpc3
${R3BROOT_SOURCE_DIR}/r3bdata/mwpcData
)

if(EXISTS "${R3BROOT_SOURCE_DIR}/sofia/")
set(WITH_SOFIA true)
set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES}
${R3BROOT_SOURCE_DIR}/sofia/sofdata/tofwData
)
endif(EXISTS "${R3BROOT_SOURCE_DIR}/sofia/")

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES ${ROOT_LIBRARY_DIR} ${FAIRROOT_LIBRARY_DIR} )

link_directories( ${LINK_DIRECTORIES})

file(GLOB SRCS 
mwpc0/*.cxx 
mwpc1/*.cxx 
mwpc2/*.cxx
digi/*.cxx
online/*.cxx
mwpc3/R3BMwpc3.cxx
mwpc3/R3BMwpc3ContFact.cxx
mwpc3/R3BMwpc3CalPar.cxx
mwpc3/R3BMwpc3Mapped2Cal.cxx
mwpc3/R3BMwpc3Mapped2CalPar.cxx
)

if(WITH_SOFIA)
file(GLOB SRCS 
${SRCS}
mwpc3/R3BMwpc3Cal2Hit.cxx
)
endif(WITH_SOFIA)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF MwpcLinkDef.h)
set(LIBRARY_NAME R3BMwpc)
set(DEPENDENCIES
  R3BBase R3BData R3BTracking)

GENERATE_LIBRARY()

add_subdirectory(geobase)
add_subdirectory(test)
