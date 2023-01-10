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

# Create a library called "libR3BCalifa" which includes the source files given
# in the array. The extension is already found. Any number of sources could be
# listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
${Boost_INCLUDE_DIR}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/passive
${R3BROOT_SOURCE_DIR}/tracking
${R3BROOT_SOURCE_DIR}/califa
${R3BROOT_SOURCE_DIR}/califa/online
${R3BROOT_SOURCE_DIR}/califa/calibration
${R3BROOT_SOURCE_DIR}/califa/ana
${R3BROOT_SOURCE_DIR}/califa/pars
${R3BROOT_SOURCE_DIR}/califa/sim
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/wrData
${R3BROOT_SOURCE_DIR}/r3bdata/califaData
${R3BROOT_SOURCE_DIR}/r3bdata/amsData
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
./sim/R3BCalifa.cxx
./sim/R3BCalifaDigitizer.cxx
./pars/R3BCalifaContFact.cxx
./pars/R3BCalifaGeometry.cxx
./pars/R3BCalifaMappingPar.cxx
./pars/R3BCalifaCrystalCalPar.cxx
./pars/R3BCalifaTotCalPar.cxx
./pars/R3BCalifaClusterPar.cxx
./pars/R3BCalifaCrystalPars4Sim.cxx
./calibration/R3BCalifaMapped2CrystalCalPar.cxx
./calibration/R3BCalifaCrystalCal2TotCalPar.cxx
./calibration/R3BCalifaMapped2CrystalCal.cxx
./calibration/R3BCalifaCrystalCal2Cluster.cxx
./calibration/R3BCalifaCrystalCal2CrystalCalPID.cxx
./online/R3BCalifaOnlineSpectra.cxx
./online/R3BCalifaJulichOnlineSpectra.cxx
./online/R3BCalifaDemoOnlineSpectra.cxx
./ana/R3BCalifaCrystalCalDataAnalysis.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF CalifaLinkDef.h)
set(LIBRARY_NAME R3BCalifa)

set(DEPENDENCIES
  Spectrum R3BBase R3BPassive R3BData R3BTracking Boost::regex)

GENERATE_LIBRARY()

add_subdirectory(geobase)
add_subdirectory(test)
