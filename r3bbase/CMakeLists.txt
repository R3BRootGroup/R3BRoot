# Create a library called "libR3B" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${R3BROOT_SOURCE_DIR}/fairtools
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase 
#${R3BROOT_SOURCE_DIR}/passive
#${R3BROOT_SOURCE_DIR}/mcstack 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
R3BModule.cxx 
R3BDetector.cxx 
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF R3BLinkDef.h)
Set(LIBRARY_NAME R3Bbase)
Set(DEPENDENCIES
    GeoBase ParBase Core Geom GenVector Physics Matrix MathCore FairTools)

GENERATE_LIBRARY()

