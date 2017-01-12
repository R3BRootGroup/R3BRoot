set(LIBRARY_NAME R3BNeulandUnpack)
set(LINKDEF NeulandUnpackLinkDef.h)

set(SRCS
        R3BNeulandTcalFill.cxx
        R3BNeulandTcal.cxx
        R3BNeulandPmt.cxx
        )
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

set(DEPENDENCIES
        R3Bbase
        R3BTCal
        R3BData
        )

set(INCLUDE_DIRECTORIES
        ${R3BROOT_SOURCE_DIR}/neuland/unpack
        ${R3BROOT_SOURCE_DIR}/r3bbase/
        ${R3BROOT_SOURCE_DIR}/r3bdata/
        ${R3BROOT_SOURCE_DIR}/r3bdata/neulandData
        ${R3BROOT_SOURCE_DIR}/r3bbase
        ${R3BROOT_SOURCE_DIR}/tcal
        )
include_directories(${INCLUDE_DIRECTORIES})

generate_library()
