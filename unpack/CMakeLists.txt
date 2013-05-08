# Create a library called "libR3BCal" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
#put here all directories where header files are located
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/r3bbase 
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/mcstack 
${CMAKE_SOURCE_DIR}/unpack
${CMAKE_SOURCE_DIR}/unpack/land
${CMAKE_SOURCE_DIR}/MbsAPI
${CMAKE_SOURCE_DIR}/r3bdata/
${CMAKE_SOURCE_DIR}/r3bdata/calData
${CMAKE_SOURCE_DIR}/r3bdata/dchData
${CMAKE_SOURCE_DIR}/r3bdata/gfiData
${CMAKE_SOURCE_DIR}/r3bdata/mtofData
${CMAKE_SOURCE_DIR}/r3bdata/tofData
${CMAKE_SOURCE_DIR}/r3bdata/landData
${CMAKE_SOURCE_DIR}/r3bdata/trackerData

)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(UNPACK_SRCS
#Put here your sourcefiles
TRootEvent.cxx
land/R3BLandUnpack.cxx
land/R3BLandRawHit.cxx
land/R3BLandRawAna.cxx
land/TLANDEvent.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h UNPACK_HEADERS "${UNPACK_SRCS}")

set(UNPACK_LINKDEF UnpackLinkDef.h)
set(UNPACK_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/UnpackDict.cxx)

ROOT_GENERATE_DICTIONARY("${UNPACK_HEADERS}" "${UNPACK_LINKDEF}" "${UNPACK_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(UNPACK_SRCS ${UNPACK_SRCS} ${UNPACK_DICTIONARY})

add_library(Unpack SHARED ${UNPACK_SRCS})
target_link_libraries(Unpack ${ROOT_LIBRARIES})
set_target_properties(Unpack PROPERTIES  ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Unpack DESTINATION ${CMAKE_BINARY_DIR}/lib)
