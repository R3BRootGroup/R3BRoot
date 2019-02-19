set(LIBRARY_NAME R3BNeulandPreexp)
set(LINKDEF NeulandPreexpLinkDef.h)

set(DEPENDENCIES R3BData R3Bbase R3BChannelAccess)

set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES}
    ${EPICS_INCLUDE_DIR}
    ${EPICS_OS_INCLUDE_DIR}
    ${R3BROOT_SOURCE_DIR}/epics
    ${R3BROOT_SOURCE_DIR}/neuland/preexp)
include_directories(${INCLUDE_DIRECTORIES})

link_directories(${EPICS_LIBRARY_DIR})

set(SRCS R3BNeulandCheckMapping.cxx R3BNeulandGainMatching.cxx R3BNeulandTimeRes.cxx)
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

generate_library()
