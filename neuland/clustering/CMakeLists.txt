set(LIBRARY_NAME R3BNeulandClustering)
set(LINKDEF NeulandClusteringLinkDef.h)


set(SRCS
	ClusteringEngine.cxx
	Neutron2DCalibr.cxx
	R3BNeulandClusterMon.cxx
	R3BNeulandClusterFinder.cxx
)
change_file_extension(*.cxx *.h HEADERS "${SRCS}")


set(DEPENDENCIES
	R3BData
)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wformat-security -pthread -march=native -O3")


set(SYSTEM_INCLUDE_DIRECTORIES
	${SYSTEM_INCLUDE_DIRECTORIES}
	${BASE_INCLUDE_DIRECTORIES}
)
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})


set(INCLUDE_DIRECTORIES
	${R3BROOT_SOURCE_DIR}/neuland
	${R3BROOT_SOURCE_DIR}/neuland/clustering
	${R3BROOT_SOURCE_DIR}/r3bdata/
	${R3BROOT_SOURCE_DIR}/r3bdata/neulandData
)
include_directories(${INCLUDE_DIRECTORIES})


set(LINK_DIRECTORIES
	${ROOT_LIBRARY_DIR}
	${Boost_LIBRARY_DIRS}
	${FAIRROOT_LIBRARY_DIR}
)
link_directories(${LINK_DIRECTORIES})

generate_library()


# Testing
enable_testing()
Set(GTEST_ROOT ${SIMPATH})
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(testClusteringEngine testClusteringEngine.cxx)
target_link_libraries(testClusteringEngine ${GTEST_BOTH_LIBRARIES})
add_test(NeulandClusteringEngineUnitTest ${EXECUTABLE_OUTPUT_PATH}/testClusteringEngine)
