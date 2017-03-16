# Create a library called "libR3BCal" which includes the source files given in
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
${R3BROOT_SOURCE_DIR}/land
${R3BROOT_SOURCE_DIR}/los
${R3BROOT_SOURCE_DIR}/r3bdata/
${R3BROOT_SOURCE_DIR}/r3bdata/caloData
${R3BROOT_SOURCE_DIR}/r3bdata/dchData
${R3BROOT_SOURCE_DIR}/r3bdata/gfiData
${R3BROOT_SOURCE_DIR}/r3bdata/mtofData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/r3bdata/landData
${R3BROOT_SOURCE_DIR}/r3bdata/neulandData
${R3BROOT_SOURCE_DIR}/r3bdata/losData
${R3BROOT_SOURCE_DIR}/r3bdata/trackerData
${R3BROOT_SOURCE_DIR}/r3bdb/landdb
${R3BROOT_SOURCE_DIR}/tcal
${R3BROOT_SOURCE_DIR}/neuland/unpack
${R3BROOT_SOURCE_DIR}/land/calibration
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

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
R3BNeutHit.cxx
calibration/R3BNeulandMappedHist.cxx
calibration/R3BNeulandMapped2CalPar.cxx
calibration/R3BNeulandMapped2Cal.cxx
calibration/R3BNeulandHitHist.cxx
#calibration/R3BNeulandCalTest.cxx
calibration/R3BNeulandHitPar.cxx
calibration/R3BNeulandCal2HitPar.cxx
calibration/R3BLandCosmic1LSQR.cxx
calibration/R3BLandCosmic1Util.cxx
calibration/R3BNeulandParFact.cxx
calibration/R3BNeulandCal2Hit.cxx
calibration/R3BNeulandHitModulePar.cxx
calibration/R3BNeulandMapped2QCalPar.cxx
calibration/R3BNeulandQCalPar.cxx
calibration/R3BNeulandHitFiller.cxx
calibration/R3BNeulandQCalFiller.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF LandLinkDef.h)
Set(LIBRARY_NAME R3BLand)
Set(DEPENDENCIES
    R3Bbase R3BData R3BPassive R3BTCal)

GENERATE_LIBRARY()

