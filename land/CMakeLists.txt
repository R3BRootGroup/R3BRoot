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
${R3BROOT_SOURCE_DIR}/land/unpack
${R3BROOT_SOURCE_DIR}/los
${R3BROOT_SOURCE_DIR}/los/unpack
${R3BROOT_SOURCE_DIR}/r3bdata/
${R3BROOT_SOURCE_DIR}/r3bdata/caloData
${R3BROOT_SOURCE_DIR}/r3bdata/dchData
${R3BROOT_SOURCE_DIR}/r3bdata/gfiData
${R3BROOT_SOURCE_DIR}/r3bdata/mtofData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/r3bdata/landData
${R3BROOT_SOURCE_DIR}/r3bdata/trackerData
${R3BROOT_SOURCE_DIR}/r3bdb/landdb
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
R3BNeuLand.cxx
R3BNeuLandv1.cxx
R3BGeoLand.cxx
R3BGeoLandPar.cxx
R3BLandContFact.cxx
R3BLandDigitizer.cxx
R3BLandDigitizerQA.cxx
R3BNeutronTracker.cxx
R3BLandData.cxx
R3BLandDigiPar.cxx
R3BConstantFraction.cxx
R3BLandDigitizer_CFD.cxx
R3BLandDigiAnalyser.cxx
R3BNeuLandCluster.cxx
R3BNeuLandClusterFinder.cxx
R3BNeutronCalibr2D.cxx
R3BPrimPart.cxx
R3BNeutronTracker2D.cxx
R3BNeutHit.cxx
unpack/R3BLandRawAna.cxx
unpack/R3BLandRawAnaS438b.cxx
unpack/R3BLandMapping.cxx
unpack/R3BLandRawHitMapped.cxx
unpack/R3BLandUnpack.cxx
unpack/R3BLandTcalFill.cxx
unpack/R3BLandParamRead.cxx
unpack/R3BLandPmt.cxx
unpack/R3BLandTcal.cxx
unpack/R3BLandTdiffFill.cxx
unpack/R3BLandTdiff.cxx
unpack/R3BLandAna.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF LandLinkDef.h)
Set(LIBRARY_NAME R3BLand)
Set(DEPENDENCIES
    R3Bbase R3BData R3BPassive MCStack R3BLandDB)

GENERATE_LIBRARY()

