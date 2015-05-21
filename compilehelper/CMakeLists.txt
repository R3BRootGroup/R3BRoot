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
${R3BROOT_SOURCE_DIR}/xball
${R3BROOT_SOURCE_DIR}/cal
${R3BROOT_SOURCE_DIR}/cal/unpack
${R3BROOT_SOURCE_DIR}/r3bdata 
${R3BROOT_SOURCE_DIR}/r3bdata/caloData
${R3BROOT_SOURCE_DIR}/r3bdata/dchData
${R3BROOT_SOURCE_DIR}/r3bdata/gfiData
${R3BROOT_SOURCE_DIR}/r3bdata/mtofData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/r3bdata/landData
${R3BROOT_SOURCE_DIR}/r3bdata/trackerData
${R3BROOT_SOURCE_DIR}/r3bdb/caldb
${R3BROOT_SOURCE_DIR}/compilehelper
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DINCLUDE_DIRECTORIES=\"${INCLUDE_DIRECTORIES};${SYSTEM_INCLUDE_DIRECTORIES}\"" )

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
#Put here your sourcefiles
R3BMacroCompiler.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF LinkDef.h)
set(LIBRARY_NAME R3BMacroCompiler)
#set(DEPENDENCIES
#    R3Bbase R3BData R3BPassive MCStack R3BXBall)

GENERATE_LIBRARY()

