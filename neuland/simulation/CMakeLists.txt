set(LIBRARY_NAME R3BNeulandSimulation)
set(LINKDEF NeulandSimulationLinkDef.h)

set(DEPENDENCIES
        R3BNeulandShared
        R3BData
        R3Bbase
        )

set(INCLUDE_DIRECTORIES
        ${INCLUDE_DIRECTORIES}
        ${R3BROOT_SOURCE_DIR}/neuland/simulation
        )
include_directories(${INCLUDE_DIRECTORIES})

set(SRCS
        R3BNeuland.cxx
        R3BNeulandMCMon.cxx
        )
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

generate_library()
