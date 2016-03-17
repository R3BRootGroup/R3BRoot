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
${R3BROOT_SOURCE_DIR}/r3bdata/caloData  
${R3BROOT_SOURCE_DIR}/r3bdata/dhcData  
${R3BROOT_SOURCE_DIR}/r3bdata/gfiData  
${R3BROOT_SOURCE_DIR}/r3bdata/mtofData  
${R3BROOT_SOURCE_DIR}/r3bdata/dtofData  
${R3BROOT_SOURCE_DIR}/r3bdata/tofData  
${R3BROOT_SOURCE_DIR}/r3bdata/vetoData  
${R3BROOT_SOURCE_DIR}/r3bdata/lumonData  
${R3BROOT_SOURCE_DIR}/r3bdata/trackerData  
${R3BROOT_SOURCE_DIR}/r3bdata/starttrackerData  
${R3BROOT_SOURCE_DIR}/r3bdata/landData  
${R3BROOT_SOURCE_DIR}/r3bdata/losData
${R3BROOT_SOURCE_DIR}/r3bdata/mfiData  
${R3BROOT_SOURCE_DIR}/r3bdata/pspData  
${R3BROOT_SOURCE_DIR}/r3bdata/neulandData
${R3BROOT_SOURCE_DIR}/r3bdata/actarData
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
xballData/R3BXBallPoint.cxx
xballData/R3BXBallCrystalHit.cxx
xballData/R3BXBallCrystalHitSim.cxx
caloData/R3BCaloCrystalHit.cxx
caloData/R3BCaloCrystalHitSim.cxx
caloData/R3BCaloRawHit.cxx
caloData/R3BCaloHit.cxx
caloData/R3BCaloHitSim.cxx
caloData/R3BCaloPoint.cxx 
caloData/R3BCaloRawHit.cxx
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
startrackerData/R3BSTaRTraPoint.cxx 
startrackerData/R3BSTaRTraHit.cxx 
startrackerData/R3BSTaRTrackerHit.cxx
startrackerData/R3BSTaRTrackerDigitHit.cxx
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
neulandData/R3BNeulandUnpackData.cxx
neulandData/R3BNeulandMappedData.cxx
neulandData/R3BNeulandCalData.cxx
pspData/R3BPspxMappedData.cxx
pspData/R3BPspxCalData.cxx
losData/R3BLosMappedData.cxx
actarData/R3BActarPoint.cxx
losData/R3BLosCalData.cxx
losData/R3BLosHitData.cxx
neulandData/R3BPaddleTamexMappedData.cxx
tofData/R3BPaddleCalData.cxx
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

