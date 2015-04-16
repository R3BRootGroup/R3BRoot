cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(${ROOT_INCLUDE_DIR} 
  ${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/r3bdata/caloData/ 
)
set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES} ${LIBRARY_OUTPUT_PATH} ${SIMPATH}/lib/ )

#set(SRCS timestitcher.cxx )

# fill list of header files from list of source files
# by exchanging the file extension
#CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_CXX_FLAGS_DEBUG "-g") 
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
 
execute_process(COMMAND root-config --libs 
  OUTPUT_VARIABLE ROOTLIBS 
  OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ROOTLIBS} -lboost_program_options -lR3BCaloData" )  

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}  -lm -lrt")
add_library( timestitcher SHARED libtimestitcher.cxx Land02TreeWrapper.cxx R3BTreeWrapper.cxx  TreeIterator.cxx  TreeWrapper.cxx )
target_link_libraries(timestitcher R3BData R3BCalo)

