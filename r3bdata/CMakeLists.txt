# Create a library called "libR3BDATA" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/r3bdata  
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${CMAKE_SOURCE_DIR}/base
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
caloData/R3BCaloHit.cxx
caloData/R3BCaloHitSim.cxx
caloData/R3BCaloPoint.cxx 
dchData/R3BDchPoint.cxx
dchData/R3BDchFullPoint.cxx 
gfiData/R3BGfiPoint.cxx 
mtofData/R3BmTofPoint.cxx  
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
landData/R3BLandPoint.cxx
landData/R3BNeuLandPoint.cxx 
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

