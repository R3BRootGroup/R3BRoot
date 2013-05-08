# Create a library called "libPassive" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/r3bbase  
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/passive  )

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(PASSIVE_SRCS
R3BCave.cxx            
R3BGeoCave.cxx        
R3BGeoMagnet.cxx      
R3BGeoPassivePar.cxx  
R3BGeoPipe.cxx        
R3BGeoVacVesselCool.cxx        
R3BGeoTarget.cxx      
R3BMagnet.cxx         
R3BGladMagnet.cxx         
R3BPassiveContFact.cxx
R3BPipe.cxx           
R3BVacVesselCool.cxx           
R3BTarget.cxx 
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${PASSIVE_SRCS}" "${INCLUDE_DIRECTORIES}" PASSIVE_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h PASSIVE_HEADERS "${PASSIVE_SRCS}")

set(PASSIVE_LINKDEF  PassiveLinkDef.h)
set(PASSIVE_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/R3BPassiveDict.cxx) 

ROOT_GENERATE_DICTIONARY("${PASSIVE_HEADERS}" "${PASSIVE_LINKDEF}" "${PASSIVE_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(PASSIVE_SRCS ${PASSIVE_SRCS} ${PASSIVE_DICTIONARY})

add_library(R3BPassive SHARED ${PASSIVE_SRCS})
target_link_libraries(R3BPassive ${ROOT_LIBRARIES})
set_target_properties(R3BPassive PROPERTIES
${FAIRROOT_LIBRARY_PROPERTIES}) 
################ install ################### 
install(TARGETS R3BPassive DESTINATION ${CMAKE_BINARY_DIR}/lib)
