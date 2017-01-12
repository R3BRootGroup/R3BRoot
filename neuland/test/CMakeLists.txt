cmake_minimum_required(VERSION 3.0)

enable_testing()
set(PROJECT_TEST_NAME NeulandUnitTests)
set(GTEST_ROOT ${SIMPATH})
find_package(GTest REQUIRED)

file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/neuland/test/*.cxx)

include_directories(
        ${GTEST_INCLUDE_DIRS}
        ${SYSTEM_INCLUDE_DIRECTORIES}
        ${BASE_INCLUDE_DIRECTORIES}
        ${R3BROOT_SOURCE_DIR}/r3bbase
        ${R3BROOT_SOURCE_DIR}/r3bdata/neulandData
        ${R3BROOT_SOURCE_DIR}/neuland/shared
        ${R3BROOT_SOURCE_DIR}/neuland/reconstruction
)

link_directories(
        ${GTEST_LIBS_DIR}
        ${ROOT_LIBRARY_DIR}
        ${FAIRROOT_LIBRARY_DIR}
        ${Boost_LIBRARY_DIRS}
)

set(TEST_DEPENDENCIES
        ${GTEST_BOTH_LIBRARIES}
        ${ROOT_LIBRARIES}
        R3Bbase
        R3BData
        R3BNeulandShared
        R3BNeulandReconstruction
        )

add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
target_link_libraries(${PROJECT_TEST_NAME} ${TEST_DEPENDENCIES})
add_test(${PROJECT_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_TEST_NAME})
