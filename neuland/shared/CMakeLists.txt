set(LIBRARY_NAME R3BNeulandShared)
set(LINKDEF NeulandSharedLinkDef.h)

set(DEPENDENCIES
        R3BData
        )

include_directories(${INCLUDE_DIRECTORIES})

set(SRCS
        ElasticScattering.cxx
        IsElastic.cxx
        R3BNeulandNeutron2DPar.cxx
        R3BNeulandGeoPar.cxx
        R3BNeulandContFact.cxx
        R3BNeulandVisualizer.cxx
        )

set(HEADERS
        ClusteringEngine.h
        ElasticScattering.h
        Filterable.h
        Operations.h
        TCAConnector.h
        Validated.h
        IsElastic.h
        R3BNeulandNeutron2DPar.h
        R3BNeulandGeoPar.h
        R3BNeulandContFact.h
        R3BNeulandVisualizer.h
        )

generate_library()


#find_package(Protobuf REQUIRED)
#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS neuland.proto)
#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS neuland_clusters.proto)
#protobuf_generate_python(PROTO_PY neuland.proto)
#set(SRCS ${SRCS} ${PROTO_SRCS})
#set(HEADERS ${HEADERS} ${PROTO_HDRS})
# Include directory such that protoc generated headers are found
#include_directories(${CMAKE_CURRENT_BINARY_DIR})
#        ${PROTOBUF_LIBRARY}
