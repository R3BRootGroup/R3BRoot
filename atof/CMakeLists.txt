# Create a library called "libR3BCal" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/base
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/passive
${R3BROOT_SOURCE_DIR}/mcstack
${R3BROOT_SOURCE_DIR}/fibers 
${R3BROOT_SOURCE_DIR}/atof
${R3BROOT_SOURCE_DIR}/r3bdata/atofData 
${R3BROOT_SOURCE_DIR}/base
${R3BROOT_SOURCE_DIR}/parbase   
${R3BROOT_SOURCE_DIR}/geobase 
${R3BROOT_SOURCE_DIR}/r3bdata  
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(R3BCAL_SRCS
#Put here your sourcefiles
R3BATof.cxx  
R3BDetPlain.cxx  
R3BGeoATof.cxx   
R3BGeoATofPar.cxx   
R3BATofContFact.cxx   
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h R3BCAL_HEADERS "${R3BCAL_SRCS}")

set(R3BCAL_LINKDEF ATofLinkDef.h)
set(R3BCAL_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/R3BATofDict.cxx)

ROOT_GENERATE_DICTIONARY("${R3BCAL_HEADERS}" "${R3BCAL_LINKDEF}" "${R3BCAL_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(R3BCAL_SRCS ${R3BCAL_SRCS} ${R3BCAL_DICTIONARY})

add_library(R3BATof SHARED ${R3BCAL_SRCS})
target_link_libraries(R3BATof ${ROOT_LIBRARIES})
set_target_properties(R3BATof PROPERTIES  ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS R3BATof DESTINATION ${CMAKE_BINARY_DIR}/lib)
