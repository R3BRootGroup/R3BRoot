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

# Create a library called "libR3BLand" which includes the source files given in 
# the array. The extension is already found. Any number of sources could be
# listed here.

set(SYSTEM_INCLUDE_DIRECTORIES ${SYSTEM_INCLUDE_DIRECTORIES} ${BASE_INCLUDE_DIRECTORIES})

set(INCLUDE_DIRECTORIES
    # put here all directories where header files are located
    ${R3BROOT_SOURCE_DIR}/r3bbase
    ${R3BROOT_SOURCE_DIR}/land
    ${R3BROOT_SOURCE_DIR}/los
    ${R3BROOT_SOURCE_DIR}/r3bdata/
    ${R3BROOT_SOURCE_DIR}/r3bdata/tofData
    ${R3BROOT_SOURCE_DIR}/r3bdata/landData
    ${R3BROOT_SOURCE_DIR}/r3bdata/neulandData
    ${R3BROOT_SOURCE_DIR}/r3bdata/losData
    ${R3BROOT_SOURCE_DIR}/tcal
    ${R3BROOT_SOURCE_DIR}/neuland/unpack)

include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES ${ROOT_LIBRARY_DIR} ${FAIRROOT_LIBRARY_DIR})

link_directories(${LINK_DIRECTORIES})

set(SRCS
    R3BLand.cxx
    R3BGeoLand.cxx
    R3BGeoLandPar.cxx
    R3BLandContFact.cxx
    R3BLandDigitizer.cxx
    R3BLandDigitizerQA.cxx
    R3BNeutronTracker.cxx
    R3BLandDigiPar.cxx
    R3BNeuLandCluster.cxx
    R3BNeuLandClusterFinder.cxx
    R3BNeutronCalibr2D.cxx
    R3BPrimPart.cxx
    R3BNeutronTracker2D.cxx
    R3BNeutHit.cxx)

# fill list of header files from list of source files by exchanging the file extension
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF LandLinkDef.h)

set(LIBRARY_NAME R3BLand)

set(DEPENDENCIES 
R3BBase R3BData R3BTCal)

generate_library()
