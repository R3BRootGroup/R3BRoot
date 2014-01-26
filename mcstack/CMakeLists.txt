# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/mcstack  
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/parbase
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
FairStack.cxx 
FairMCTrack.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")
SET( HEADERS ${HEADERS} FairDetectorList.h )

set(LINKDEF  MCStackLinkDef.h)
set(LIBRARY_NAME MCStack)
set(DEPENDENCIES
    Base FairTools)

GENERATE_LIBRARY()

