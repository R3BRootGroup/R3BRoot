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
${R3BROOT_SOURCE_DIR}/startracker
${R3BROOT_SOURCE_DIR}/startracker/unpack
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/xballData
${R3BROOT_SOURCE_DIR}/r3bdata/califaData
${R3BROOT_SOURCE_DIR}/r3bdata/dchData
${R3BROOT_SOURCE_DIR}/r3bdata/gfiData
${R3BROOT_SOURCE_DIR}/r3bdata/mtofData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/r3bdata/landData 
${R3BROOT_SOURCE_DIR}/r3bdata/startrackerData)


include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
#Put here your sourcefiles
R3BSTaRTra.cxx  
R3BGeoSTaRTra.cxx   
R3BGeoSTaRTraPar.cxx   
R3BSTaRTraContFact.cxx  
R3BSTaRTraHitFinder.cxx 
R3BSTaRTraDigit.cxx
R3BSTaRTraDigiPar.cxx
R3BSTaRTrackerEvent.cxx
unpack/R3BStarTrackRawHit.cxx
unpack/R3BStarTrackUnpack.cxx
unpack/R3BStarTrackUnpackPar.cxx
unpack/R3BStarTrackRawAna.cxx
#unpack/R3BStarTrackSortRawAna.cxx
unpack/R3BStarTrackRecTS.cxx
unpack/R3BStarTrackOrderTS.cxx
unpack/R3BStarTrackStripAna.cxx
unpack/R3BStarTrackCalib.cxx
unpack/R3BStarTrackCalibPar.cxx
unpack/R3BStarTrackCalibParFinder.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF STaRTraLinkDef.h)
set(LIBRARY_NAME R3BSTaRTra)
set(DEPENDENCIES
    R3Bbase R3BPassive MCStack R3BData)

GENERATE_LIBRARY()

