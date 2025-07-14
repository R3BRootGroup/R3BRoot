# Input parameters: -DBUILD_J=1 -DTEST_NAME="R3BRoot CI/CD" -DTEST_MODEL="Experimental" -DBUILD_TYPE= -DEXTRA_FLAGS= -DSITE_NAME=

# mandatory cmake variable definition
set(CTEST_SOURCE_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}/../..")
set(CTEST_BINARY_DIRECTORY "${CTEST_SOURCE_DIRECTORY}/build")
include(${CTEST_SOURCE_DIRECTORY}/CTestConfig.cmake)

include(${CTEST_SOURCE_DIRECTORY}/cmake/CI_CD/Macros.cmake)
if(NOT DEFINED CTEST_CMAKE_GENERATOR)
    set(CTEST_CMAKE_GENERATOR Ninja)
endif()
ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})

set_input_options()

check_test_model(${ctest_model})

enable_launcher(1)

cdash_start()

cdash_configure()

cdash_build()

cdash_test()

cdash_submit()

print_error()
