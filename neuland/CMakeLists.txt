# ROOT5 is not supported by several submodules
if(ROOT_VERSION_MAJOR GREATER 5)
	add_subdirectory(reconstruction)
	add_subdirectory(clustering)
	add_subdirectory(digitizing)
else(ROOT_VERSION_MAJOR GREATER 5)
	message(WARNING "Not building important NeuLAND modules because ROOT is too old. Upgrade to ROOT6, muppet!")
endif(ROOT_VERSION_MAJOR GREATER 5)

set(LIBRARY_NAME R3BNeuland)
set(LINKDEF NeulandLinkDef.h)

set(SRCS
	R3BNeuland.cxx
	R3BNeulandNeutron2DPar.cxx
	R3BNeulandGeoPar.cxx
	R3BNeulandContFact.cxx
	R3BNeulandMCMon.cxx
	R3BNeulandVisualizer.cxx
	unpack/R3BNeulandTcalFill.cxx
	unpack/R3BNeulandTcal.cxx
	unpack/R3BNeulandPmt.cxx
)
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

set(DEPENDENCIES
	R3Bbase R3BTCal R3BData
)

# -pthread for gtest
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wformat-security -pthread -march=native -O3")


set(SYSTEM_INCLUDE_DIRECTORIES
	${SYSTEM_INCLUDE_DIRECTORIES}
	${BASE_INCLUDE_DIRECTORIES}
)
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(INCLUDE_DIRECTORIES
	${R3BROOT_SOURCE_DIR}/neuland
	${R3BROOT_SOURCE_DIR}/neuland/unpack
	${R3BROOT_SOURCE_DIR}/r3bbase/
	${R3BROOT_SOURCE_DIR}/r3bdata/
	${R3BROOT_SOURCE_DIR}/r3bdata/neulandData
	${R3BROOT_SOURCE_DIR}/r3bbase
	${R3BROOT_SOURCE_DIR}/tcal
)
include_directories(${INCLUDE_DIRECTORIES})


set(LINK_DIRECTORIES
	${ROOT_LIBRARY_DIR}
	${FAIRROOT_LIBRARY_DIR}
)
link_directories(${LINK_DIRECTORIES})


generate_library()


# Testing
enable_testing()
Set(GTEST_ROOT ${SIMPATH})
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

If (CMAKE_SYSTEM_NAME MATCHES Linux)
	Set(DEPENDENCIES ${DEPENDENCIES} rt)
EndIf (CMAKE_SYSTEM_NAME MATCHES Linux)
add_executable(testNeulandNeutron2DPar test/testNeulandNeutron2DPar.cxx)
target_link_libraries(testNeulandNeutron2DPar ${GTEST_BOTH_LIBRARIES} ${ROOT_LIBRARIES} R3BNeuland ${DEPENDENCIES})
add_test(NeulandNeutron2DParUnitTest ${EXECUTABLE_OUTPUT_PATH}/testNeulandNeutron2DPar)
