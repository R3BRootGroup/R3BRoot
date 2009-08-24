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
${R3BROOT_SOURCE_DIR}/tof
${R3BROOT_SOURCE_DIR}/chimera
${R3BROOT_SOURCE_DIR}/r3bdata/calData
${R3BROOT_SOURCE_DIR}/r3bdata/dchData
${R3BROOT_SOURCE_DIR}/r3bdata/gfiData
${R3BROOT_SOURCE_DIR}/r3bdata/mtofData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/r3bdata/chimeraData
${R3BROOT_SOURCE_DIR}/r3bdata/landData
${R3BROOT_SOURCE_DIR}/r3bdata/trackerData ${R3BROOT_SOURCE_DIR}/base
${R3BROOT_SOURCE_DIR}/parbase   ${R3BROOT_SOURCE_DIR}/geobase )

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(R3BCAL_SRCS
#Put here your sourcefiles
R3BChimera.cxx  
R3BGeoChimera.cxx   
R3BGeoChimeraPar.cxx   
R3BChimeraContFact.cxx   
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h R3BCAL_HEADERS "${R3BCAL_SRCS}")

set(R3BCAL_LINKDEF ChimeraLinkDef.h)
set(R3BCAL_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/R3BChimeraDict.cxx)

ROOT_GENERATE_DICTIONARY("${R3BCAL_HEADERS}" "${R3BCAL_LINKDEF}" "${R3BCAL_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(R3BCAL_SRCS ${R3BCAL_SRCS} ${R3BCAL_DICTIONARY})

add_library(R3BChimera SHARED ${R3BCAL_SRCS})
target_link_libraries(R3BChimera ${ROOT_LIBRARIES})
set_target_properties(R3BChimera PROPERTIES  ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS R3BChimera DESTINATION ${CMAKE_BINARY_DIR}/lib)
