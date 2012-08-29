# Create a library called "libSOFIAMCStack" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/mcstack  )

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/example/mcstack

)
 
link_directories( ${LINK_DIRECTORIES})

set(SOFIAMCSTACK_SRCS
FairStack.cxx 
FairMCTrack.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${SOFIAMCSTACK_SRCS}" "${INCLUDE_DIRECTORIES}" SOFIAMCSTACK_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h SOFIAMCSTACK_HEADERS "${SOFIAMCSTACK_SRCS}")
SET( SOFIAMCSTACK_HEADERS ${SOFIAMCSTACK_HEADERS} FairDetectorList.h )

set(SOFIAMCSTACK_LINKDEF  MCStackLinkDef.h)
set(SOFIAMCSTACK_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/SOFIAMCStackDict.cxx) 

ROOT_GENERATE_DICTIONARY("${SOFIAMCSTACK_HEADERS}" "${SOFIAMCSTACK_LINKDEF}" "${SOFIAMCSTACK_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(SOFIAMCSTACK_SRCS ${SOFIAMCSTACK_SRCS} ${SOFIAMCSTACK_DICTIONARY})

add_library(SOFIAMCStack SHARED ${SOFIAMCSTACK_SRCS})
target_link_libraries(SOFIAMCStack ${ROOT_LIBRARIES})
set_target_properties(SOFIAMCStack PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS SOFIAMCStack DESTINATION ${CMAKE_BINARY_DIR}/lib)
