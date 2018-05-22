# Create a library called "libR3BDATA" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual")

Set(SYSTEM_INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
${R3BROOT_SOURCE_DIR}/r3bdata  
${R3BROOT_SOURCE_DIR}/r3bdata/xballData  
${R3BROOT_SOURCE_DIR}/r3bdata/califaData
${R3BROOT_SOURCE_DIR}/r3bdata/dhcData  
${R3BROOT_SOURCE_DIR}/r3bdata/gfiData  
${R3BROOT_SOURCE_DIR}/r3bdata/mtofData  
${R3BROOT_SOURCE_DIR}/r3bdata/dtofData  
${R3BROOT_SOURCE_DIR}/r3bdata/tofData  
${R3BROOT_SOURCE_DIR}/r3bdata/vetoData  
${R3BROOT_SOURCE_DIR}/r3bdata/lumonData  
${R3BROOT_SOURCE_DIR}/r3bdata/trackerData  
${R3BROOT_SOURCE_DIR}/r3bdata/startrackData  
${R3BROOT_SOURCE_DIR}/r3bdata/landData  
${R3BROOT_SOURCE_DIR}/r3bdata/losData
${R3BROOT_SOURCE_DIR}/r3bdata/mfiData
${R3BROOT_SOURCE_DIR}/r3bdata/fibData
${R3BROOT_SOURCE_DIR}/r3bdata/pspData
${R3BROOT_SOURCE_DIR}/r3bdata/neulandData
${R3BROOT_SOURCE_DIR}/r3bdata/actarData
${R3BROOT_SOURCE_DIR}/r3bdata/strawtubesData
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
R3BMCStack.cxx
R3BMCTrack.cxx
R3BHit.cxx
xballData/R3BXBallPoint.cxx
xballData/R3BXBallCrystalHit.cxx
xballData/R3BXBallCrystalHitSim.cxx
califaData/R3BCalifaCrystalCalData.cxx
califaData/R3BCalifaCrystalCalDataSim.cxx
califaData/R3BCalifaMappedData.cxx
califaData/R3BCalifaHitData.cxx
califaData/R3BCalifaHitDataSim.cxx
califaData/R3BCalifaPoint.cxx
dchData/R3BDchPoint.cxx
dchData/R3BDchFullPoint.cxx
gfiData/R3BGfiPoint.cxx
mtofData/R3BmTofPoint.cxx
dtofData/R3BdTofPoint.cxx
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
mtofData/R3BmTofDigi.cxx
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
neulandData/R3BNeulandMappedData.cxx
neulandData/R3BNeulandCalData.cxx
neulandData/R3BNeulandPoint.cxx
neulandData/R3BNeulandDigi.cxx
neulandData/R3BNeulandCluster.cxx
neulandData/R3BNeulandNeutron.cxx
neulandData/R3BNeulandPixel.cxx
pspData/R3BPspxMappedData.cxx
pspData/R3BPspxPrecalData.cxx
pspData/R3BPspxCalData.cxx
pspData/R3BPspxHitData.cxx
losData/R3BLosMappedData.cxx
actarData/R3BActarPoint.cxx
strawtubesData/R3BStrawtubesMappedData.cxx
strawtubesData/R3BStrawtubesCalData.cxx
strawtubesData/R3BStrawtubesHitData.cxx
losData/R3BLosCalData.cxx
losData/R3BLosHitData.cxx
neulandData/R3BPaddleTamexMappedData.cxx
tofData/R3BPaddleCalData.cxx
tofData/R3BTofdHitData.cxx
ptofData/R3BPtofHit.cxx
fibData/R3BBunchedFiberCalData.cxx
fibData/R3BBunchedFiberHitData.cxx
fibData/R3BBunchedFiberMappedData.cxx
fibData/R3BFi4Point.cxx
fibData/R3BFi4CalItem.cxx
fibData/R3BFi4HitItem.cxx
fibData/R3BFibMappedData.cxx
fibData/R3BFibCalData.cxx
)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")
SET(HEADERS ${HEADERS} R3BDetectorList.h )

set(LINKDEF  R3BDataLinkDef.h)
set(LIBRARY_NAME R3BData)
set(DEPENDENCIES
    Base FairTools)

GENERATE_LIBRARY()
