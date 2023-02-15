
##############################################################################
#   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2022-2023 Members of R3B Collaboration                     #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

# Create a library called "libR3BRpc" which includes the source files given
# in the array. The extension is already found. Any number of sources could be
# listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/tracking
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/rpcData
${R3BROOT_SOURCE_DIR}/r3bdata/losData
${R3BROOT_SOURCE_DIR}/r3bdata/wrData
${R3BROOT_SOURCE_DIR}/rpc
${R3BROOT_SOURCE_DIR}/rpc/sim
${R3BROOT_SOURCE_DIR}/rpc/digi
${R3BROOT_SOURCE_DIR}/rpc/pars
${R3BROOT_SOURCE_DIR}/rpc/calibration
${R3BROOT_SOURCE_DIR}/rpc/online
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
#Put here your sourcefiles
sim/R3BRpc.cxx
pars/R3BRpcContFact.cxx
#R3BRpcDigitizer.cxx
calibration/R3BRpcMapped2PreCalPar.cxx
calibration/R3BRpcMapped2PreCal.cxx
calibration/R3BRpcPreCal2CalPar.cxx
calibration/R3BRpcPreCal2Cal.cxx
pars/R3BRpcTotCalPar.cxx
calibration/R3BRpcCal2Hit.cxx
calibration/R3BRpcCal2HitPar.cxx
pars/R3BRpcHitPar.cxx
pars/R3BRpcPars4Sim.cxx
./online/R3BRpcOnlineSpectra.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF RpcLinkDef.h)
set(LIBRARY_NAME R3BRpc)

set(DEPENDENCIES
    R3BBase R3BTracking R3BData R3BTCal)

GENERATE_LIBRARY()

if(BUILD_GEOMETRY)
  add_subdirectory(geobase)
endif()
add_subdirectory(test)
