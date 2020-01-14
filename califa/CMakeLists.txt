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

# Create a library called "libR3BCalifa" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/passive
${R3BROOT_SOURCE_DIR}/xball
${R3BROOT_SOURCE_DIR}/califa
${R3BROOT_SOURCE_DIR}/califa/unpack
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/wrData
${R3BROOT_SOURCE_DIR}/r3bdata/califaData
${R3BROOT_SOURCE_DIR}/r3bdata/dchData
${R3BROOT_SOURCE_DIR}/r3bdata/gfiData
${R3BROOT_SOURCE_DIR}/r3bdata/mtofData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/r3bdata/landData
${R3BROOT_SOURCE_DIR}/r3bdata/trackerData
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
R3BCalifa.cxx
R3BCalifaGeometry.cxx
R3BCalifaCrystalCal2Hit.cxx
R3BCalifaHitPar.cxx
R3BCalifaMappingPar.cxx
R3BCalifaContFact.cxx
R3BCalifaDigitizer.cxx
R3BCalifaOnlineSpectra.cxx
unpack/R3BCalifaMapped2CrystalCal.cxx
unpack/R3BCalifaCrystalCalDataAnalysis.cxx
unpack/R3BCalifaMapped2CrystalCalPar.cxx
unpack/R3BCalifaCrystalCal2CrystalCalPID.cxx
unpack/R3BCalifaCrystalCalPar.cxx
unpack/R3BCalifaDemoOnlineSpectra.cxx
#unpack/R3BCalifaDUCalPar.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF CalifaLinkDef.h)
set(LIBRARY_NAME R3BCalifa)
set(DEPENDENCIES
    Spectrum R3Bbase R3BData R3BPassive R3BXBall)

if(FAIRDB_FOUND)
set(DEPENDENCIES
    ${DEPENDENCIES} FairDB)
endif(FAIRDB_FOUND)


GENERATE_LIBRARY()
