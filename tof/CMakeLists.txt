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
${R3BROOT_SOURCE_DIR}/tof
${R3BROOT_SOURCE_DIR}/los
${R3BROOT_SOURCE_DIR}/tcal
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/losData
${R3BROOT_SOURCE_DIR}/r3bdata/calData
${R3BROOT_SOURCE_DIR}/r3bdata/dchData
${R3BROOT_SOURCE_DIR}/r3bdata/gfiData
${R3BROOT_SOURCE_DIR}/r3bdata/mtofData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/r3bdata/landData
${R3BROOT_SOURCE_DIR}/r3bdata/neulandData
${R3BROOT_SOURCE_DIR}/r3bdata/trackerData
${R3BROOT_SOURCE_DIR}/r3bdata/ptofData  )

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
#Put here your sourcefiles
R3BTof.cxx  
R3BGeoTof.cxx   
R3BGeoTofPar.cxx   
R3BTofContFact.cxx 
R3BTofDigiPar.cxx
R3BTofDigitizer.cxx
R3BTof2pDigiPar.cxx
R3BTof2pDigitizer.cxx  
#R3BTofUnpack.cxx
R3BTofdMapped2TCal.cxx
R3BTofdMapped2TCalPar.cxx
R3BTofdCal2HitPar.cxx
R3BTofdCal2Hit.cxx
R3BTofdHitModulePar.cxx
R3BTofdHitPar.cxx
R3BPtofMapped2CalPar.cxx
R3BPtofMapped2Cal.cxx
calibration/R3BPtofCal2HitPar.cxx
calibration/R3BPtofCal2Hit.cxx
calibration/R3BPtofHitPar.cxx
calibration/R3BPtofParFact.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF TofLinkDef.h)
set(LIBRARY_NAME R3BTof)
set(DEPENDENCIES
    Spectrum R3Bbase R3BPassive R3BData R3BTCal)
    
GENERATE_LIBRARY()

