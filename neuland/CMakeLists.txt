set(LIBRARY_NAME R3BNeuland)
set(LINKDEF NeulandLinkDef.h)

set(SRCS
	R3BNeulandMCMon.cxx
	R3BNeulandDigiMon.cxx
	R3BNeulandVisualizer.cxx
	R3BNeulandDigitizer.cxx
	digitizing/DigitizingEngine.cxx
)
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

set(DEPENDENCIES
  R3Bbase
)

# TODO: -Wno-ignored-qualifiers for R3BLandDigiPar
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-ignored-qualifiers")

set(SYSTEM_INCLUDE_DIRECTORIES
	${SYSTEM_INCLUDE_DIRECTORIES}
	${BASE_INCLUDE_DIRECTORIES}
)
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(INCLUDE_DIRECTORIES
	${R3BROOT_SOURCE_DIR}/land/
	${R3BROOT_SOURCE_DIR}/neuland/
	${R3BROOT_SOURCE_DIR}/neuland/digitizing
	${R3BROOT_SOURCE_DIR}/r3bdata/
	${R3BROOT_SOURCE_DIR}/r3bdata/landData
)
include_directories(${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
	${ROOT_LIBRARY_DIR}
	${FAIRROOT_LIBRARY_DIR}
)
link_directories(${LINK_DIRECTORIES})

generate_library()
