# Create a library called "libR3B" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase 
${R3BROOT_SOURCE_DIR}/r3bdata/landData
${R3BROOT_SOURCE_DIR}/r3bdata/caloData
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
R3BModule.cxx 
R3BDetector.cxx 
R3BEventHeader.cxx
R3BEventHeaderUnpack.cxx
R3BTimeStampUnpack.cxx
R3BLmdSource.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF R3BLinkDef.h)

Set(DEPENDENCIES
    GeoBase ParBase MbsAPI Base FairTools R3BData Core Geom GenVector Physics Matrix MathCore)

Set(LIBRARY_NAME R3Bbase)

GENERATE_LIBRARY()

