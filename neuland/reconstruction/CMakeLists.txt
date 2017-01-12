set(LIBRARY_NAME R3BNeulandReconstruction)
set(LINKDEF NeulandReconstructionLinkDef.h)

set(DEPENDENCIES
        R3BNeulandShared
        R3BData
        )

set(INCLUDE_DIRECTORIES
        ${INCLUDE_DIRECTORIES}
        ${R3BROOT_SOURCE_DIR}/neuland/reconstruction
        )
include_directories(${INCLUDE_DIRECTORIES})

set(SRCS
        R3BNeulandNeutronReconstruction.cxx
        R3BNeulandNeutronReconstructionMon.cxx
        R3BNeulandNeutronReconstructionStatistics.cxx
        R3BNeulandNeutronClusters.cxx
        ReconstructionEngine.cxx
        ReconstructionOperation.cxx
        ClusterScoring.cxx
        RecoTDR.cxx
        Neutron2DCalibr.cxx
        )
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

generate_library()
