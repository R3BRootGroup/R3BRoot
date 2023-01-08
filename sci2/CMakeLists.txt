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

# Create a library called "libR3BSci2" which includes the source files given in
# the array. The extension is already found. Any number of sources could be
# listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/sci2
${R3BROOT_SOURCE_DIR}/sci2/params
${R3BROOT_SOURCE_DIR}/sci2/calib
${R3BROOT_SOURCE_DIR}/sci2/online
${R3BROOT_SOURCE_DIR}/r3bdata/
${R3BROOT_SOURCE_DIR}/r3bdata/losData
${R3BROOT_SOURCE_DIR}/r3bdata/sci2Data
${R3BROOT_SOURCE_DIR}/tcal
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
calib/R3BSci2Mapped2Tcal.cxx
calib/R3BSci2Mapped2Cal.cxx
params/R3BSci2Mapped2CalPar.cxx
calib/R3BSci2Tcal2Hit.cxx
params/R3BSci2HitPar.cxx
params/R3BSci2ContFact.cxx
online/R3BOnlineSpectraSci2.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF Sci2LinkDef.h)
Set(LIBRARY_NAME R3BSci2)
Set(DEPENDENCIES
    R3BBase R3BData R3BTCal)

GENERATE_LIBRARY()
