set(LIBRARY_NAME R3BNeulandDigitizing)
set(LINKDEF NeulandDigitizingLinkDef.h)

set(DEPENDENCIES
        R3BNeulandShared
        R3BData
        )

set(INCLUDE_DIRECTORIES
        ${INCLUDE_DIRECTORIES}
        ${R3BROOT_SOURCE_DIR}/neuland/digitizing
        )
include_directories(${INCLUDE_DIRECTORIES})

set(SRCS
        DigitizingEngine.cxx
        DigitizingTacQuila.cxx
        R3BNeulandDigiMon.cxx
        R3BNeulandDigitizer.cxx
        )
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

generate_library()
