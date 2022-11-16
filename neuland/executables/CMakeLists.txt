set(EXE_NAME neuland)
set(DEPENDENCIES R3BNeulandShared R3BNeulandSimulation R3BPassive R3BData R3BBase Gen)

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES}
${R3BROOT_SOURCE_DIR}/passive
${R3BROOT_SOURCE_DIR}/neuland/executables
${R3BROOT_SOURCE_DIR}/neuland/simulation
)

include_directories(${INCLUDE_DIRECTORIES})
set(SRCS
    neuland.cxx
    NeulandSimulator.cxx)

GENERATE_EXECUTABLE()

