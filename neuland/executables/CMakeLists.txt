set(EXE_NAME neulandSim)
set(DEPENDENCIES
    R3BNeulandShared
    R3BNeulandSimulation
    R3BPassive
    R3BData
    R3BBase
    Gen
    ${Geant4_LIBRARIES}
    ${Geant4VMC_LIBRARIES}
    ExTestDetector
    Boost::program_options)

set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES} ${R3BROOT_SOURCE_DIR}/passive ${R3BROOT_SOURCE_DIR}/neuland/executables
    ${R3BROOT_SOURCE_DIR}/neuland/simulation ${Geant4_INCLUDE_DIRS} ${Geant4VMC_INCLUDE_DIRS})

message(STATUS "include dir: ${Geant4_INCLUDE_DIRS}")
include_directories(${INCLUDE_DIRECTORIES})
set(SRCS neulandSim.cxx)

generate_executable()

set(EXE_NAME neulandAna)
set(DEPENDENCIES
    R3BNeulandShared
    R3BNeulandDigitizing
    R3BPassive
    R3BData
    R3BBase
    Gen
    Boost::program_options)

set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES} ${R3BROOT_SOURCE_DIR}/passive ${R3BROOT_SOURCE_DIR}/neuland/executables
    ${R3BROOT_SOURCE_DIR}/neuland/digitizing ${R3BROOT_SOURCE_DIR}/neuland/calibration)

include_directories(${INCLUDE_DIRECTORIES})
set(SRCS neulandAna.cxx)

generate_executable()
