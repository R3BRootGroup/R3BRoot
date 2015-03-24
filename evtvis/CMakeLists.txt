# Create a library called "libR3BEvtVis" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/cal 
${CMAKE_SOURCE_DIR}/r3bdata/caloData
${CMAKE_SOURCE_DIR}/eventdisplay
${CMAKE_SOURCE_DIR}/r3bdata
${CMAKE_SOURCE_DIR}/evtvis
)


include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
R3BEventManager.cxx
R3BEventManagerEditor.cxx
R3BCaloEventDisplay.cxx
R3BCaloHitEventDisplay.cxx
R3BMCTracks.cxx
R3BIonName.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF R3BEventDisplayLinkDef.h)
set(LIBRARY_NAME R3BEvtVis)
set(DEPENDENCIES
    Field MCStack R3BData R3BCalo Geane EventDisplay)

GENERATE_LIBRARY()

