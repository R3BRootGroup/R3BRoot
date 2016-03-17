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
${R3BROOT_SOURCE_DIR}/r3bdata/trackerData
${R3BROOT_SOURCE_DIR}/r3bdb/landdb
${R3BROOT_SOURCE_DIR}/tcal
${R3BROOT_SOURCE_DIR}/neuland/unpack
${R3BROOT_SOURCE_DIR}/land/unpack
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
#unpack/R3BLandRawAna.cxx
#unpack/R3BLandRawAnaS438b.cxx
#unpack/R3BLandMapping.cxx
#unpack/R3BLandUnpack.cxx
unpack/R3BNeulandMapped2CalPar.cxx
unpack/R3BLandParamRead.cxx
unpack/R3BNeulandMapped2Cal.cxx
#unpack/R3BLandAna.cxx
#unpack/R3BNeulandCalTest.cxx
unpack/R3BNeulandHitPar.cxx
unpack/R3BNeulandCal2HitPar.cxx
unpack/R3BLandCosmic1LSQR.cxx
unpack/R3BLandCosmic1Util.cxx
unpack/R3BNeulandParFact.cxx
unpack/R3BNeulandCal2Hit.cxx
unpack/R3BNeulandHitModulePar.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF LandLinkDef.h)
Set(LIBRARY_NAME R3BLand)
Set(DEPENDENCIES
    R3Bbase R3BData R3BPassive R3BTCal)

GENERATE_LIBRARY()

