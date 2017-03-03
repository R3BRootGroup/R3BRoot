# Create a library called "libR3BPSP" which includes the source files
#given in # the array .  # The extension is already found.  Any number
#of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES 
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase 
${R3BROOT_SOURCE_DIR}/passive
${R3BROOT_SOURCE_DIR}/fi4
${R3BROOT_SOURCE_DIR}/fi6
${R3BROOT_SOURCE_DIR}/fiber
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/tcal
${R3BROOT_SOURCE_DIR}/r3bdata/calData
# ${R3BROOT_SOURCE_DIR}/r3bdata/dchData
${R3BROOT_SOURCE_DIR}/r3bdata/pspData
${R3BROOT_SOURCE_DIR}/r3bdata/fi4Data
${R3BROOT_SOURCE_DIR}/r3bdata/fibData
${R3BROOT_SOURCE_DIR}/r3bdata/mtofData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/r3bdata/landData
${R3BROOT_SOURCE_DIR}/r3bdata/trackerData)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories(${LINK_DIRECTORIES})

set(SRCS
R3BFi6.cxx  
#R3BGeoFi4.cxx   
# R3BGeoFi4Par.cxx   
# R3BFi4ContFact.cxx
#R3BFi4DigiPar.cxx  
R3BFi6Mapped2CalPar.cxx
R3BFi6Mapped2Cal.cxx
R3BFi6Cal2Hit.cxx
#R3BFi4Cal2Hit.cxx
)

CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF Fi6LinkDef.h)
Set(LIBRARY_NAME R3BFi6)
Set(DEPENDENCIES
    R3Bbase R3BData R3BPassive MCStack R3BTCal R3BBunchedFiber)

GENERATE_LIBRARY()
