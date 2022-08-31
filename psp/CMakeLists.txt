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

# Create a library called "libR3BPSP" which includes the source files
#given in # the array .  # The extension is already found.  Any number
#of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/psp
${R3BROOT_SOURCE_DIR}/psp/par
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/calData
# ${R3BROOT_SOURCE_DIR}/r3bdata/dchData
${R3BROOT_SOURCE_DIR}/r3bdata/pspData
${R3BROOT_SOURCE_DIR}/r3bdata/mtofData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/r3bdata/landData
${R3BROOT_SOURCE_DIR}/r3bdata/trackerData
${R3BROOT_SOURCE_DIR}/tracking)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
#Put here your sourcefiles
R3BPsp.cxx
R3BGeoPsp.cxx
# R3BGeoPspPar.cxx
# R3BPspContFact.cxx
R3BPspDigiPar.cxx
R3BPspDigitizer.cxx
# R3BPspxMapped2Cal.cxx
R3BPspxMapped2Precal.cxx
R3BPspxPrecal2Cal.cxx
R3BPspxCal2Hit.cxx
R3BPspxOnlineSpectra.cxx
par/R3BPspxMappedPar.cxx
par/R3BPspxPrecalPar.cxx
par/R3BPspxCalPar.cxx
par/R3BPspxHitPar.cxx
par/R3BPspxContFact.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF PspLinkDef.h)
Set(LIBRARY_NAME R3BPsp)
Set(DEPENDENCIES
  R3BBase R3BData R3BTracking)

GENERATE_LIBRARY()
