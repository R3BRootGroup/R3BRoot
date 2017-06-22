# Create a library called "libR3BEvtVis" 

Set(SYSTEM_INCLUDE_DIRECTORIES 
${SYSTEM_INCLUDE_DIRECTORIES}
${GEANT3_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
${R3BROOT_SOURCE_DIR}/field 
${R3BROOT_SOURCE_DIR}/geane
${R3BROOT_SOURCE_DIR}/califa 
${R3BROOT_SOURCE_DIR}/r3bdata/califaData
${R3BROOT_SOURCE_DIR}/eventdisplay
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/evtvis
)


include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
R3BEventManager.cxx
R3BEventManagerEditor.cxx
R3BCalifaEventDisplay.cxx
R3BCalifaHitEventDisplay.cxx
R3BMCTracks.cxx
R3BIonName.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF R3BEventDisplayLinkDef.h)
set(LIBRARY_NAME R3BEvtVis)
set(DEPENDENCIES
    Field MCStack R3BData R3BCalifa Geane EventDisplay)

GENERATE_LIBRARY()

