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
${R3BROOT_SOURCE_DIR}/xball
${R3BROOT_SOURCE_DIR}/califa
${R3BROOT_SOURCE_DIR}/tracker
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/califaData
${R3BROOT_SOURCE_DIR}/r3bdata/dchData
${R3BROOT_SOURCE_DIR}/r3bdata/gfiData
${R3BROOT_SOURCE_DIR}/r3bdata/mtofData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/r3bdata/landData
${R3BROOT_SOURCE_DIR}/r3bdata/trackerData
${R3BROOT_SOURCE_DIR}/tracker/unpackams
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
R3BTra.cxx  
R3BGeoTra.cxx   
R3BGeoTraPar.cxx   
R3BTraContFact.cxx
R3BTraDigiPar.cxx
R3BTraDigitizer.cxx
R3BTargetDigiPar.cxx
R3BTargetDigitizer.cxx
R3BTra2pDigiPar.cxx
R3BTra2pDigitizer.cxx
R3BTraFraDigiPar.cxx
R3BTraFraDigitizer.cxx
R3BTarget2pDigiPar.cxx
R3BTarget2pDigitizer.cxx   
R3BTraHitFinder.cxx
unpackams/R3BAmsStripCalPar.cxx
unpackams/R3BAmsMapped2StripCalPar.cxx
unpackams/R3BAmsMapped2StripCal.cxx
unpackams/R3BAmsStripCal2Hit.cxx
unpackams/R3BAmsOnlineSpectra.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF TraLinkDef.h)
set(LIBRARY_NAME R3BTra)
set(DEPENDENCIES
    R3Bbase R3BData)

if(FAIRDB_FOUND)
set(DEPENDENCIES
    ${DEPENDENCIES} FairDB)
endif(FAIRDB_FOUND)



GENERATE_LIBRARY()


