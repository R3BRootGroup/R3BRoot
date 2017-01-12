set(LIBRARY_NAME R3BNeulandClustering)
set(LINKDEF NeulandClusteringLinkDef.h)

set(DEPENDENCIES
        R3BNeulandShared
        R3BData
        )

set(INCLUDE_DIRECTORIES
        ${INCLUDE_DIRECTORIES}
        ${R3BROOT_SOURCE_DIR}/neuland/clustering
        )
include_directories(${INCLUDE_DIRECTORIES})

set(SRCS
        R3BNeulandClusterMon.cxx
        R3BNeulandClusterFinder.cxx
        )
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

generate_library()
