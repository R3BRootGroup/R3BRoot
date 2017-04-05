set(LIBRARY_NAME R3BNeulandReconstruction)
set(LINKDEF NeulandReconstructionLinkDef.h)


set(SRCS
	R3BNeulandNeutronReconstruction.cxx
	R3BNeulandNeutronReconstructionMon.cxx
	ReconstructionEngine.cxx
	Reconstruction2DCalibr.cxx
	ReconstructionEGate.cxx
	# ReconstructionNeuralNetworkMultiplicity.cxx
)
change_file_extension(*.cxx *.h HEADERS "${SRCS}")


find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS neuland.proto)
#protobuf_generate_python(PROTO_PY neuland.proto)
set(SRCS ${SRCS} ${PROTO_SRCS})
set(HEADERS ${HEADERS} ${PROTO_HDRS})
# Include directory such that protoc generated headers are found
include_directories(${CMAKE_CURRENT_BINARY_DIR})


set(DEPENDENCIES
  R3BData ${PROTOBUF_LIBRARY} FairMQ
)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wformat-security -pthread -march=native -O3")


set(SYSTEM_INCLUDE_DIRECTORIES
	${SYSTEM_INCLUDE_DIRECTORIES}
	${BASE_INCLUDE_DIRECTORIES}
)
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})


set(INCLUDE_DIRECTORIES
	${R3BROOT_SOURCE_DIR}/neuland
	${R3BROOT_SOURCE_DIR}/neuland/reconstruction
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
