# Create a library called "libR3BEvtVis" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/mcstack
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
)
 
link_directories( ${LINK_DIRECTORIES})

set(R3BEVEDISP_SRCS
R3BEventManager.cxx
R3BEventManagerEditor.cxx
R3BCaloEventDisplay.cxx
R3BCaloHitEventDisplay.cxx
R3BMCTracks.cxx
R3BIonName.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${R3BEVEDISP_SRCS}" "${INCLUDE_DIRECTORIES}" R3BEVEDISP_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h R3BEVEDISP_HEADERS "${R3BEVEDISP_SRCS}")

set(R3BEVEDISP_LINKDEF R3BEventDisplayLinkDef.h)
set(R3BEVEDISP_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/R3BEventDisplayDict.cxx) 

ROOT_GENERATE_DICTIONARY("${R3BEVEDISP_HEADERS}" "${R3BEVEDISP_LINKDEF}" "${R3BEVEDISP_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(R3BEVEDISP_SRCS ${R3BEVEDISP_SRCS} ${R3BEVEDISP_DICTIONARY})

add_library(R3BEvtVis SHARED ${R3BEVEDISP_SRCS})
target_link_libraries(R3BEvtVis ${ROOT_LIBRARIES})
set_target_properties(R3BEvtVis PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS R3BEvtVis DESTINATION ${CMAKE_BINARY_DIR}/lib)
