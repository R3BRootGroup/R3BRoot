# Create a library called "libR3BGFI" which includes the source files
#given in # the array .  # The extension is already found.  Any number
#of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
#put here all directories where header files are located
${FAIRROOT_SOURCE_DIR}/base
${FAIRROOT_SOURCE_DIR}/passive
${FAIRROOT_SOURCE_DIR}/mcstack
${FAIRROOT_SOURCE_DIR}/gfi
${FAIRROOT_SOURCE_DIR}/r3bdata/calData
${FAIRROOT_SOURCE_DIR}/r3bdata/dchData
${FAIRROOT_SOURCE_DIR}/r3bdata/gfiData
${FAIRROOT_SOURCE_DIR}/r3bdata/mtofData
${FAIRROOT_SOURCE_DIR}/r3bdata/tofData
${FAIRROOT_SOURCE_DIR}/r3bdata/landData
${FAIRROOT_SOURCE_DIR}/r3bdata/trackerData
${FAIRROOT_SOURCE_DIR}/base
${FAIRROOT_SOURCE_DIR}/parbase  
${FAIRROOT_SOURCE_DIR}/geobase )

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(R3BCAL_SRCS
#Put here your sourcefiles
R3BGfi.cxx  
R3BGeoGfi.cxx   
R3BGeoGfiPar.cxx   
R3BGfiContFact.cxx   
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h R3BCAL_HEADERS "${R3BCAL_SRCS}")

set(R3BCAL_LINKDEF GfiLinkDef.h)
set(R3BCAL_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/R3BGfiDict.cxx)

ROOT_GENERATE_DICTIONARY("${R3BCAL_HEADERS}" "${R3BCAL_LINKDEF}" "${R3BCAL_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(R3BCAL_SRCS ${R3BCAL_SRCS} ${R3BCAL_DICTIONARY})

add_library(R3BGfi SHARED ${R3BCAL_SRCS})
target_link_libraries(R3BGfi ${ROOT_LIBRARIES})
set_target_properties(R3BGfi PROPERTIES  ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS R3BGfi DESTINATION ${CMAKE_BINARY_DIR}/lib)
