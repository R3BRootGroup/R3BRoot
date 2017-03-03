# Create a library called "libR3BPSP" which includes the source files
#given in # the array .  # The extension is already found.  Any number
#of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES 
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
${R3BROOT_SOURCE_DIR}/r3bdata/fibData
${R3BROOT_SOURCE_DIR}/fiber
${R3BROOT_SOURCE_DIR}/tcal
)

include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories(${LINK_DIRECTORIES})

set(SRCS
R3BBunchedFiberMapped2CalPar.cxx
R3BBunchedFiberMapped2Cal.cxx
R3BBunchedFiberCal2Hit.cxx
)

CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF BunchedFiberLinkDef.h)
Set(LIBRARY_NAME R3BBunchedFiber)
set(DEPENDENCIES
    R3Bbase R3BPassive R3BData R3BTCal)

GENERATE_LIBRARY()
