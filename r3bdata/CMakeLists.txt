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

# Create a library called "libR3BData" which includes the source files given in the array.
# The extension is already found. Any number of sources could be listed here.

# Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual") Set(CMAKE_CXX_FLAGS
# "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wformat-security -march=native")

set(SYSTEM_INCLUDE_DIRECTORIES ${SYSTEM_INCLUDE_DIRECTORIES} ${BASE_INCLUDE_DIRECTORIES})

set(INCLUDE_DIRECTORIES
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/r3bdata/xballData
${R3BROOT_SOURCE_DIR}/r3bdata/califaData
${R3BROOT_SOURCE_DIR}/r3bdata/wrData
${R3BROOT_SOURCE_DIR}/r3bdata/beammonitorData
${R3BROOT_SOURCE_DIR}/r3bdata/dhcData
${R3BROOT_SOURCE_DIR}/r3bdata/gfiData
${R3BROOT_SOURCE_DIR}/r3bdata/mtofData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/r3bdata/pdcData
${R3BROOT_SOURCE_DIR}/r3bdata/vetoData
${R3BROOT_SOURCE_DIR}/r3bdata/lumonData
${R3BROOT_SOURCE_DIR}/r3bdata/trackerData
${R3BROOT_SOURCE_DIR}/r3bdata/startrackData
${R3BROOT_SOURCE_DIR}/r3bdata/landData
${R3BROOT_SOURCE_DIR}/r3bdata/roluData
${R3BROOT_SOURCE_DIR}/r3bdata/losData
${R3BROOT_SOURCE_DIR}/r3bdata/sci2Data
${R3BROOT_SOURCE_DIR}/r3bdata/sci8Data
${R3BROOT_SOURCE_DIR}/r3bdata/mfiData
${R3BROOT_SOURCE_DIR}/r3bdata/fibData
${R3BROOT_SOURCE_DIR}/r3bdata/pspData
${R3BROOT_SOURCE_DIR}/r3bdata/neulandData
${R3BROOT_SOURCE_DIR}/r3bdata/actarData
${R3BROOT_SOURCE_DIR}/r3bdata/strawtubesData
${R3BROOT_SOURCE_DIR}/r3bdata/amsData
${R3BROOT_SOURCE_DIR}/r3bdata/footData
${R3BROOT_SOURCE_DIR}/r3bdata/alpideData
${R3BROOT_SOURCE_DIR}/r3bdata/sci2Data
${R3BROOT_SOURCE_DIR}/r3bdata/sci8Data
${R3BROOT_SOURCE_DIR}/r3bdata/musicData
${R3BROOT_SOURCE_DIR}/r3bdata/sampData
${R3BROOT_SOURCE_DIR}/r3bdata/sfibData
${R3BROOT_SOURCE_DIR}/r3bdata/frsData
${R3BROOT_SOURCE_DIR}/r3bdata/rpcData
${R3BROOT_SOURCE_DIR}/r3bdata/mwpcData
${R3BROOT_SOURCE_DIR}/r3bdata/twimData
${R3BROOT_SOURCE_DIR}/r3bdata/musliData
)

include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES ${ROOT_LIBRARY_DIR} ${FAIRROOT_LIBRARY_DIR})

link_directories(${LINK_DIRECTORIES})

set(SRCS
R3BMCStack.cxx
R3BMCTrack.cxx
wrData/R3BWRData.cxx
trloiiData/R3BTrloiiData.cxx
R3BHit.cxx
R3BTrack.cxx
R3BDouble.cxx
xballData/R3BXBallPoint.cxx
xballData/R3BXBallCrystalHit.cxx
xballData/R3BXBallCrystalHitSim.cxx
califaData/R3BCalifaCrystalCalData.cxx
califaData/R3BCalifaMappedData.cxx
califaData/R3BCalifaHitData.cxx
califaData/R3BCalifaPoint.cxx
dchData/R3BDchPoint.cxx
dchData/R3BDchFullPoint.cxx
gfiData/R3BGfiPoint.cxx
mtofData/R3BmTofPoint.cxx
mtofData/R3BmTofDigi.cxx
tofData/R3BTofPoint.cxx
vetoData/R3BVetoPoint.cxx
lumonData/ELILuMonPoint.cxx
trackerData/R3BTraPoint.cxx
trackerData/R3BTraHit.cxx
trackerData/R3BSitrackerHit.cxx
trackerData/R3BTrackerHit.cxx
startrackData/R3BStartrackPoint.cxx
startrackData/R3BStartrackHit.cxx
#startrackData/R3BStartrackerHit.cxx
startrackData/R3BStartrackerDigitHit.cxx
startrackData/R3BStartrackMappedData.cxx
startrackData/R3BStartrackCalData.cxx
landData/R3BLandPoint.cxx
landData/R3BLandDigi.cxx
landData/R3BLandFirstHits.cxx
landData/R3BLandHit.cxx
landData/R3BPaddleCrystalHit.cxx
landData/R3BNeutronTrack.cxx
dchData/R3BDchDigi.cxx
dchData/R3BDch2pDigi.cxx
tofData/R3BTofDigi.cxx
tofData/R3BTof2pDigi.cxx
gfiData/R3BGfiDigi.cxx
trackerData/R3BTraDigi.cxx
trackerData/R3BTra2pDigi.cxx
trackerData/R3BTraFraDigi.cxx
trackerData/R3BTargetDigi.cxx
trackerData/R3BTarget2pDigi.cxx
mfiData/R3BMfiPoint.cxx
mfiData/R3BMfiDigi.cxx
pspData/R3BPspPoint.cxx
pspData/R3BPspDigi.cxx
neulandData/R3BNeulandTacquilaMappedData.cxx
neulandData/R3BNeulandCalData.cxx
neulandData/R3BNeulandPoint.cxx
neulandData/R3BNeulandHit.cxx
neulandData/R3BNeulandMultiplicity.cxx
neulandData/R3BNeulandCluster.cxx
neulandData/R3BNeulandNeutron.cxx
pspData/R3BPspxMappedData.cxx
pspData/R3BPspxPrecalData.cxx
pspData/R3BPspxCalData.cxx
pspData/R3BPspxHitData.cxx
losData/R3BLosMappedData.cxx
losData/R3BLosCalData.cxx
losData/R3BLosTCalData.cxx
losData/R3BLosHitData.cxx
roluData/R3BRoluMappedData.cxx
roluData/R3BRoluCalData.cxx
roluData/R3BRoluHitData.cxx
sci2Data/R3BSci2MappedData.cxx
sci2Data/R3BSci2TcalData.cxx
sci2Data/R3BSci2CalData.cxx
sci2Data/R3BSci2HitData.cxx
sci8Data/R3BSci8MappedData.cxx
sci8Data/R3BSci8CalData.cxx
sci8Data/R3BSci8HitData.cxx
actarData/R3BActarPoint.cxx
strawtubesData/R3BStrawtubesMappedData.cxx
strawtubesData/R3BStrawtubesCalData.cxx
strawtubesData/R3BStrawtubesHitData.cxx
neulandData/R3BPaddleTamexMappedData.cxx
tofData/R3BTofdMappedData.cxx
tofData/R3BTofdCalData.cxx
tofData/R3BTofdHitData.cxx
tofData/R3BPaddleCalData.cxx
tofData/R3BTofdPoint.cxx
tofData/R3BTofdDigi.cxx
tofData/R3BPtofHit.cxx
tofiData/R3BTofiMappedData.cxx
tofiData/R3BTofiCalData.cxx
tofiData/R3BTofiHitData.cxx
tofiData/R3BTofiPoint.cxx
tofiData/R3BTofiDigi.cxx
pdcData/R3BPdcMappedData.cxx
pdcData/R3BPdcCalData.cxx
pdcData/R3BPdcHitData.cxx
fibData/R3BBunchedFiberCalData.cxx
fibData/R3BBunchedFiberHitData.cxx
fibData/R3BBunchedFiberMappedData.cxx
fibData/R3BFi4Point.cxx
fibData/R3BFi4CalItem.cxx
fibData/R3BFi4HitItem.cxx
fibData/R3BFiberMappedData.cxx
fibData/R3BFibMappedData.cxx
fibData/R3BFibCalData.cxx
fibData/R3BFibPoint.cxx
fibData/R3BFiberMAPMTCalData.cxx
fibData/R3BFiberMAPMTHitData.cxx
fibData/R3BFiberMAPMTMappedData.cxx
amsData/R3BAmsMappedData.cxx
amsData/R3BAmsStripCalData.cxx
amsData/R3BAmsHitData.cxx
beammonitorData/R3BBeamMonitorMappedData.cxx
musicData/R3BMusicPoint.cxx
musicData/R3BMusicMappedData.cxx
musicData/R3BMusicCalData.cxx
musicData/R3BMusicHitData.cxx
sampData/R3BSamplerMappedData.cxx
sampData/R3BSampLosMappedData.cxx
sfibData/R3BSfibMappedData.cxx
sfibData/R3BSfibCalData.cxx
sfibData/R3BSfibHitData.cxx
frsData/R3BFrsData.cxx
footData/R3BFootMappedData.cxx
footData/R3BFootCalData.cxx
footData/R3BFootHitData.cxx
alpideData/R3BAlpidePoint.cxx
alpideData/R3BAlpideMappedData.cxx
alpideData/R3BAlpideCalData.cxx
alpideData/R3BAlpideHitData.cxx
rpcData/R3BRpcMappedData.cxx
rpcData/R3BRpcPreCalData.cxx
rpcData/R3BRpcCalData.cxx
rpcData/R3BRpcHitData.cxx
rpcData/R3BRpcPoint.cxx
mwpcData/R3BMwpcPoint.cxx
mwpcData/R3BMwpcMappedData.cxx
mwpcData/R3BMwpcCalData.cxx
mwpcData/R3BMwpcHitData.cxx
twimData/R3BTwimPoint.cxx
twimData/R3BTwimMappedData.cxx
twimData/R3BTwimCalData.cxx
twimData/R3BTwimHitData.cxx
musliData/R3BMusliMappedData.cxx
musliData/R3BMusliCalData.cxx
musliData/R3BMusliHitData.cxx
)

# fill list of header files from list of source files by exchanging the file extension
change_file_extension(*.cxx *.h HEADERS "${SRCS}")
set(HEADERS ${HEADERS} R3BDetectorList.h)

set(LINKDEF DataLinkDef.h)
set(LIBRARY_NAME R3BData)
set(DEPENDENCIES Base FairTools ${VMCLIB} FairLogger::FairLogger)

generate_library()
