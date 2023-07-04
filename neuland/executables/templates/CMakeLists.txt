set(EXE_NAME ex_digi)
set(DEPENDENCIES
    R3BNeulandShared
    R3BNeulandDigitizing
    R3BNeulandClustering
    R3BPassive
    R3BData
    R3BBase
    Gen
    Boost::program_options)

set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES} ${R3BROOT_SOURCE_DIR}/passive ${R3BROOT_SOURCE_DIR}/neuland/executables
    ${R3BROOT_SOURCE_DIR}/neuland/digitizing ${R3BROOT_SOURCE_DIR}/neuland/calibration)

include_directories(${INCLUDE_DIRECTORIES})
set(SRCS ex_digi.cxx)

generate_executable()
