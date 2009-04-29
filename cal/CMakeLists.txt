# Create a library called "libR3BCal" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase 
${R3BROOT_SOURCE_DIR}/passive 
${R3BROOT_SOURCE_DIR}/mcstack 
${R3BROOT_SOURCE_DIR}/calcrystal
${R3BROOT_SOURCE_DIR}/cal 
${R3BROOT_SOURCE_DIR}/r3bdata/caloData
${R3BROOT_SOURCE_DIR}/r3bdata/dchData
${R3BROOT_SOURCE_DIR}/r3bdata/gfiData
${R3BROOT_SOURCE_DIR}/r3bdata/mtofData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/r3bdata/landData
${R3BROOT_SOURCE_DIR}/r3bdata/trackerData  ${R3BROOT_SOURCE_DIR}/base
${R3BROOT_SOURCE_DIR}/parbase   ${R3BROOT_SOURCE_DIR}/geobase )

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(R3BCALO_SRCS
#Put here your sourcefiles
R3BCalo.cxx  
R3BGeoCalo.cxx   
R3BGeoCaloPar.cxx   
R3BCaloContFact.cxx   
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h R3BCALO_HEADERS "${R3BCALO_SRCS}")

set(R3BCALO_LINKDEF CaloLinkDef.h)
set(R3BCALO_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/R3BCaloDict.cxx)

ROOT_GENERATE_DICTIONARY("${R3BCALO_HEADERS}" "${R3BCALO_LINKDEF}" "${R3BCALO_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(R3BCALO_SRCS ${R3BCALO_SRCS} ${R3BCALO_DICTIONARY})

add_library(R3BCalo SHARED ${R3BCALO_SRCS})
target_link_libraries(R3BCalo ${ROOT_LIBRARIES})
set_target_properties(R3BCalo PROPERTIES  ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS R3BCalo DESTINATION ${CMAKE_BINARY_DIR}/lib)
