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

# Create a library called "libR3BPassive" which includes the source files given 
# in the array. The extension is already found. Any number of sources could be
# listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/field
${R3BROOT_SOURCE_DIR}/passive
${R3BROOT_SOURCE_DIR}/tracking)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
R3BCave.cxx
R3BGeoCave.cxx
R3BGeoPassivePar.cxx
R3BGeoTarget.cxx
R3BAladinMagnet.cxx
R3BGladMagnet.cxx
R3BNeutronWindowAndSomeAir.cxx
R3BPassiveContFact.cxx
R3BPipe.cxx
R3BVacVesselCool.cxx
R3BTarget.cxx
R3BCollimator.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF PassiveLinkDef.h)
Set(LIBRARY_NAME R3BPassive)
Set(DEPENDENCIES
    R3BBase Field R3BTracking)

GENERATE_LIBRARY()
