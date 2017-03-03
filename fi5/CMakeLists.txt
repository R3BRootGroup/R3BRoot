# Create a library called "libR3BPSP" which includes the source files
#given in # the array .  # The extension is already found.  Any number
#of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES 
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/fi5
${R3BROOT_SOURCE_DIR}/fiber
${R3BROOT_SOURCE_DIR}/r3bbase 
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/fibData
${R3BROOT_SOURCE_DIR}/tcal
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
R3BFi5.cxx  
R3BFi5Cal2Hit.cxx
R3BFi5Mapped2Cal.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF Fi5LinkDef.h)
Set(LIBRARY_NAME R3BFi5)
Set(DEPENDENCIES
    R3BBunchedFiber)

GENERATE_LIBRARY()

