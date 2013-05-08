# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

# Check for gcc version, since for gcc 4 one has to add friend-injection
# as option to compile FairPlutoGenerator
#EXEC_PROGRAM( gcc ARGS "-dumpversion" OUTPUT_VARIABLE GCC_VERSION )
#STRING(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]" "\\1" req_gcc_major_vers "${GCC_VERSION}")
#STRING(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9]" "\\1" req_gcc_minor_vers "${GCC_VERSION}")
#IF(${req_gcc_major_vers} MATCHES "4" AND NOT ${req_gcc_minor_vers} MATCHES "0")
#  SET_SOURCE_FILES_PROPERTIES(FairPlutoGenerator.cxx PROPERTIES COMPILE_FLAGS
#  "-ffriend-injection")
#ENDIF(${req_gcc_major_vers} MATCHES "4" AND NOT ${req_gcc_minor_vers} MATCHES "0")



set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${PLUTO_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/generators 
${CMAKE_SOURCE_DIR}/r3bgen
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${PLUTO_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(GENERATORS_SRCS
R3BPrimaryGenerator.cxx
R3BSpecificGenerator.cxx
R3BReadKinematics.cxx
R3BCDGenerator.cxx
R3BBeamInfo.cxx
R3BBackTracking.cxx
R3BBackTrackingStorageState.cxx
R3BAsciiGenerator.cxx
R3BCosmicGenerator.cxx
R3Bp2pGenerator.cxx
R3BLandGenerator.cxx
R3BCALIFATestGenerator.cxx
R3BAsciiIQMDGen.cxx
R3BAsciiUrQMDGen.cxx
R3BIonGenerator.cxx

)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${GENERATORS_SRCS}" "${INCLUDE_DIRECTORIES}" GENERATORS_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h GENERATORS_HEADERS "${GENERATORS_SRCS}")

set(GENERATORS_LINKDEF R3BGenLinkDef.h)
set(GENERATORS_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/R3BGenDict.cxx)


ROOT_GENERATE_DICTIONARY("${GENERATORS_HEADERS}" "${GENERATORS_LINKDEF}" "${GENERATORS_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(GENERATORS_SRCS ${GENERATORS_SRCS}  ${GENERATORS_DICTIONARY})


add_library(R3BGen SHARED ${GENERATORS_SRCS})
#target_link_libraries(R3BGen ${ROOT_LIBRARIES} Pluto)
set_target_properties(R3BGen PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS R3BGen DESTINATION ${CMAKE_BINARY_DIR}/lib)
