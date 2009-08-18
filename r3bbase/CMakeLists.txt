# Create a library called "libR3B" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase 
#${R3BROOT_SOURCE_DIR}/passive
#${R3BROOT_SOURCE_DIR}/mcstack 
${R3BROOT_SOURCE_DIR}/base
#${R3BROOT_SOURCE_DIR}/parbase 
#${R3BROOT_SOURCE_DIR}/geobase 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(R3B_SRCS
#Put here your sourcefiles
R3BModule.cxx 
R3BDetector.cxx 
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h R3B_HEADERS "${R3B_SRCS}")

set(R3B_LINKDEF R3BLinkDef.h)
set(R3B_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/R3BDict.cxx)

ROOT_GENERATE_DICTIONARY("${R3B_HEADERS}" "${R3B_LINKDEF}" "${R3B_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(R3B_SRCS ${R3B_SRCS} ${R3B_DICTIONARY})

add_library(R3Bbase SHARED ${R3B_SRCS}) 
target_link_libraries(R3Bbase ${ROOT_LIBRARIES})
set_target_properties(R3Bbase PROPERTIES 
${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS R3Bbase DESTINATION ${CMAKE_BINARY_DIR}/lib)
