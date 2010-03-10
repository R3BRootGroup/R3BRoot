# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${R3BROOT_SOURCE_DIR}/base  
${R3BROOT_SOURCE_DIR}/field  
${R3BROOT_SOURCE_DIR}/parbase  )

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(FIELD_SRCS
R3BFieldConst.cxx     
R3BFieldContFact.cxx    
R3BFieldMap.cxx
R3BFieldPar.cxx          
R3BFieldCreator.cxx
R3BGladFieldMap.cxx
R3BFieldInterp.cxx
R3BAladinFieldMap.cxx  )

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${FIELD_SRCS}" "${INCLUDE_DIRECTORIES}" FIELD_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h FIELD_HEADERS "${FIELD_SRCS}")

set(FIELD_LINKDEF FieldLinkDef.h)
set(FIELD_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/FieldDict.cxx) 

ROOT_GENERATE_DICTIONARY("${FIELD_HEADERS}" "${FIELD_LINKDEF}" "${FIELD_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(FIELD_SRCS ${FIELD_SRCS} ${FIELD_DICTIONARY})

add_library(Field SHARED ${FIELD_SRCS})
target_link_libraries(Field ${ROOT_LIBRARIES})
set_target_properties(Field PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Field DESTINATION ${CMAKE_BINARY_DIR}/lib)
