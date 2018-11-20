# The name of our project is “R3BBROOT”.  CMakeLists files in this project can refer to the root
# source directory of the project as ${R3BROOT_SOURCE_DIR} or as ${CMAKE_SOURCE_DIR} and to the root
# binary directory of the project as ${R3BROOT_BINARY_DIR} or ${CMAKE_BINARY_DIR}. This difference
# is is important for the base classes which are in R3BROOT and PANDAROOT.

# Check if cmake has the required version
cmake_minimum_required(VERSION 2.8.2 FATAL_ERROR)

# CMP0025   Compiler id for Apple Clang is now AppleClang. CMP0042   MACOSX_RPATH is enabled by
# default.

foreach(p
        CMP0025 # CMake 3.0
        CMP0042 # CMake 3.0
        )
    if(POLICY ${p})
        cmake_policy(SET ${p} NEW)
    endif()
endforeach()

# Set name of our project to "R3BROOT". Has to be done after check of cmake version
project(R3BROOT)

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/macros")
    message(
        FATAL_ERROR
            "\nmacros repository is missing in the R3BRoot source directory\ncd ../R3BRoot\ngit clone https://github.com/R3BRootGroup/macros.git"
        )
endif(NOT EXISTS "${PROJECT_SOURCE_DIR}/macros")

# Check if the FairRoot is still in source, since Jan 2014 FairRoot should be installed separately
find_path(FAIRBASE NAMES FairRun.h PATHS ${CMAKE_SOURCE_DIR}/base/steer NO_DEFAULT_PATH)

if(FAIRBASE)
    message(STATUS "Found FAIRBASE")
    set(FAIRBASE ${FAIRBASE})
else(FAIRBASE)
    message(STATUS "NOT Found FAIRBASE")
    if(FAIRROOT_ROOT)
        set(FAIRROOTPATH ${FAIRROOT_ROOT})
    else(FAIRROOT_ROOT)
        if(NOT DEFINED ENV{FAIRROOTPATH})
            message(
                FATAL_ERROR
                    "You did not define the environment variable FAIRROOTPATH which is needed to find FairRoot. Please set this variable and execute cmake again."
                )
        endif(NOT DEFINED ENV{FAIRROOTPATH})
        set(FAIRROOTPATH $ENV{FAIRROOTPATH})
    endif(FAIRROOT_ROOT)

endif(FAIRBASE)

# added a variable to point to r3broot path in the system
set(R3BROOTPATH ${CMAKE_INSTALL_PREFIX})

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules" ${CMAKE_MODULE_PATH})
if(NOT FAIRROOT_ROOT)
    set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules_old" ${CMAKE_MODULE_PATH})
endif(NOT FAIRROOT_ROOT)

# Check if the compiler support specific C++11 features Up to now this is only a check since the
# code does not use any of the features of the new standard

if(FAIRROOTPATH)
    set(CheckSrcDir "${FAIRROOTPATH}/share/fairbase/cmake/checks")
else(FAIRROOTPATH)
    set(CheckSrcDir "${CMAKE_SOURCE_DIR}/cmake/checks")
endif(FAIRROOTPATH)

if(FAIRROOTPATH)
    find_package(FairRoot REQUIRED)
endif(FAIRROOTPATH)

include(CheckCXX11Features)

# Load some basic macros which are needed later on
include(FairMacros)
include(R3BMacros)
include(WriteConfigFile_r3b)
include(Dart)
include(CheckCompiler)
include(WriteEnvCache)

# R3BRoot uses from 10.08.15 C++11, so we need an compiler which supports C++11 Check if the used
# compiler support C++11. If not stop with an error message Check also if FairSoft and FairRoot have
# been compiled with C++11 support
if(NOT _HAS_CXX11_FLAG)
    message(
        FATAL_ERROR
            "The used C++ compiler (${CMAKE_CXX_COMPILER}) does not support C++11. R3BRoot can only be compiled with compilers supporting C++11. Please install such an compiler."
        )
endif()

if(NOT FAIRROOT_ROOT)
    execute_process(COMMAND $ENV{SIMPATH}/bin/fairsoft-config --cxxflags
                    OUTPUT_VARIABLE _res_fairsoft_config
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(FIND ${_res_fairsoft_config} "-std=c++11" POS_C++11)
    string(FIND ${_res_fairsoft_config} "-std=c++1y" POS_C++1y)
    if(${POS_C++11} EQUAL -1 AND ${POS_C++1y} EQUAL -1)
        message(
            FATAL_ERROR
                "FairSoft wasn't compiled with c++11 or c++1y support. Please recompile FairSoft with a compiler which supports c++11 or c++1y."
            )
    endif()

    execute_process(COMMAND $ENV{FAIRROOTPATH}/bin/fairroot-config --fairsoft_path
                    OUTPUT_VARIABLE _simpath
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    remove_trailing_slash($ENV{SIMPATH})
    set(_simpath ${_ret_val})
    remove_trailing_slash(${_simpath})
    set(_fairroot_config ${_ret_val})
    string(COMPARE EQUAL
                   ${_simpath}
                   ${_fairroot_config}
                   _same_fairsoft)
    if(NOT _same_fairsoft)
        message(STATUS "FairSoft version used for FairRoot compilation: ${_fairroot_config}")
        message(STATUS "FairSoft version now (defined by SIMPATH): ${_simpath}")
        message(FATAL_ERROR "Both versions must be the same.")
    endif()
endif(NOT FAIRROOT_ROOT)

# Check the compiler and set the compile and link flags
check_compiler()

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(INCLUDE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/include")
set(VMCWORKDIR ${CMAKE_SOURCE_DIR})

option(USE_PATH_INFO "Information from PATH and LD_LIBRARY_PATH are
used." OFF)
if(USE_PATH_INFO)
    set(PATH $ENV{PATH})
    if(APPLE)
        set(LD_LIBRARY_PATH $ENV{DYLD_LIBRARY_PATH})
    else(APPLE)
        set(LD_LIBRARY_PATH $ENV{LD_LIBRARY_PATH})
    endif(APPLE)
endif(USE_PATH_INFO)

# Check if the user wants to build the project in the source directory
check_out_of_source_build()

# Check if we are on an UNIX system. If not stop with an error message
if(NOT UNIX)
    message(
        FATAL_ERROR
            "You're not on an UNIX system. The project was up to now only tested on UNIX systems, so we break here. If you want to go on please edit the CMakeLists.txt in the source directory."
        )
endif(NOT UNIX)

# Check if necessary environment variables are set If not stop execution
if(NOT FAIRROOT_ROOT)
    if(NOT R3BROOT_MINIMAL)
        if(NOT DEFINED ENV{SIMPATH})
            message(
                FATAL_ERROR
                    "You did not define the environment variable SIMPATH which is nedded to find the external packages. Please set this variable and execute cmake again."
                )
        endif(NOT DEFINED ENV{SIMPATH})
        string(REGEX MATCHALL
                     "[^:]+"
                     PATH
                     $ENV{PATH})
        set(SIMPATH $ENV{SIMPATH})
    else(NOT R3BROOT_MINIMAL)
        if(NOT DEFINED ENV{ROOTSYS})
            message(
                FATAL_ERROR
                    "You did not define the environment variable ROOTSYS which is nedded for the installation of the minimal version of FairRoot. Please set this variable and execute cmake again."
                )
        endif(NOT DEFINED ENV{ROOTSYS})
    endif(NOT R3BROOT_MINIMAL)
endif(NOT FAIRROOT_ROOT)

# Check if the external packages are installed into a separate install directory
check_external_package_install_dir()

if(NOT FAIRROOT_ROOT)
    if(FAIRSOFT_EXTERN)
        set(BOOST_ROOT ${SIMPATH})
        set(GSL_DIR ${SIMPATH})
    else(FAIRSOFT_EXTERN)
        set(BOOST_ROOT ${SIMPATH}/basics/boost)
        set(GSL_DIR ${SIMPATH}/basics/gsl)
    endif(FAIRSOFT_EXTERN)
endif(NOT FAIRROOT_ROOT)

set(Boost_NO_SYSTEM_PATHS TRUE)
set(Boost_NO_BOOST_CMAKE TRUE)
message("-- Looking for Boost ...")
# If an older version of boost is found both of the variables below are cached and in a second cmake
# run, a good boost version is found even if the version is to old. To overcome this problem both
# variables are cleared before checking for boost.
unset(Boost_INCLUDE_DIR CACHE)
unset(Boost_LIBRARY_DIRS CACHE)
find_package(Boost 1.41
             COMPONENTS thread
                        system
                        timer
                        program_options)
if(Boost_FOUND)
    set(Boost_Avail 1)
else(Boost_FOUND)
    set(Boost_Avail 0)
endif(Boost_FOUND)

if(SIMPATH)
  set(CMAKE_PREFIX_PATH ${SIMPATH} ${CMAKE_PREFIX_PATH})
endif()

find_package(FairLogger 1.1.0 REQUIRED)

setbasicvariables()

# Check if FairRoot was compiled with FairDB option
find_library(FAIRDB_FOUND FairDB ${FAIRROOTPATH}/lib NO_DEFAULT_PATH)
if(FAIRDB_FOUND)
    message("Found FairDB ...")
    add_definitions(-DWITH_FAIRDB)
    set(DEFINITIONS "${DEFINITIONS} -DWITH_FAIRDB")
else(FAIRDB_FOUND)
    message("Building without dbase ...")
endif(FAIRDB_FOUND)

# searches for needed packages REQUIRED means that cmake will stop if this packages are not found
# For example the framework can run without GEANT4, but ROOT is mandatory
if(R3BROOT_MINIMAL)
    message("******* Install minimal FairRoot Version ********")
    find_package(ROOT 6 REQUIRED)
else(R3BROOT_MINIMAL)
    find_package(ROOT 6 REQUIRED)
    # find_package(PLUTO REQUIRED) find_package(GENERATORS REQUIRED)
    if(FAIRROOT_ROOT)

        message(STATUS "Looking for GEANT3...")
        find_package(Geant3 REQUIRED)
        if(Geant3_DIR)
            set(GEANT3_INCLUDE_DIR ${Geant3_INCLUDE_DIRS})
            set(GEANT3_LIBRARIES ${Geant3_LIBRARIES})
            set(GEANT3_LIBRARY_DIR ${Geant3_DIR}/..)
            set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${GEANT3_LIBRARY_DIR})
            find_path(GEANT3_SYSTEM_DIR
                      NAMES data
                      PATHS ${Geant3_DIR}/../../share/geant3
                      NO_DEFAULT_PATH)
            set(GEANT3_FOUND TRUE)
            message(STATUS "Looking for GEANT3... - found ${GEANT3_LIBRARIES}")
        endif(Geant3_DIR)

        message(STATUS "Looking for GEANT4...")
        find_package(Geant4 REQUIRED)
        if(Geant4_DIR)
            set(GEANT4_INCLUDE_DIR ${Geant4_INCLUDE_DIRS})
            set(GEANT4_LIBRARY_DIR "${Geant4_DIR}/..")
            set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${GEANT4_LIBRARY_DIR})
            set(GEANT4_SHARE_DIR "${Geant4_DIR}/../../share")
            set(GEANT4_FOUND TRUE)
            message(STATUS "Looking for GEANT4... - found ${GEANT4_LIBRARY_DIR}")
            set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/cmake/modules_old" ${CMAKE_MODULE_PATH})
            find_package(GEANT4DATA)
        endif(Geant4_DIR)

        message(STATUS "Looking for GEANT4VMC...")
        find_package(Geant4VMC REQUIRED)
        if(Geant4VMC_DIR)
            set(GEANT4VMC_INCLUDE_DIR ${Geant4VMC_INCLUDE_DIRS})
            set(GEANT4VMC_LIBRARY_DIR "${Geant4VMC_DIR}/..")
            set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${GEANT4VMC_LIBRARY_DIR})
            set(GEANT4VMC_SHARE_DIR "${Geant4VMC_DIR}/../../share")
            find_path(GEANT4VMC_MACRO_DIR
                      NAMES g4libs.C
                      PATHS ${GEANT4VMC_SHARE_DIR}/Geant4VMC-3.3.0/examples/macro
                      NO_DEFAULT_PATH)
            set(GEANT4VMC_FOUND TRUE)
            message(STATUS "Looking for GEANT4VMC... - found ${GEANT4VMC_LIBRARY_DIR}")
        endif(Geant4VMC_DIR)

        message(STATUS "Looking for VGM...")
        find_package(VGM)
        if(VGM_DIR)
            set(VGM_INCLUDE_DIR ${VGM_INCLUDE_DIRS})
            set(VGM_LIBRARY_DIR "${VGM_DIR}/..")
            set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${VGM_LIBRARY_DIR})
            set(VGM_FOUND TRUE)
            message(STATUS "Looking for VGM... - found ${VGM_LIBRARY_DIR}")
        endif()
        # find_package(CLHEP REQUIRED)
    else(FAIRROOT_ROOT)
        find_package(GEANT3 REQUIRED)
        find_package(GEANT4 REQUIRED)
        find_package(GEANT4DATA)
        find_package(GEANT4VMC REQUIRED)
        find_package(CLHEP REQUIRED)
    endif(FAIRROOT_ROOT)
    # find_package(RuleChecker)
endif(R3BROOT_MINIMAL)

# set a variable which should be used in all CMakeLists.txt Defines all basic include directories
# from fairbase
setbasicvariables()

# Set the library version in the main CMakeLists.txt
set(R3BROOT_MAJOR_VERSION 0)
set(R3BROOT_MINOR_VERSION 0)
set(R3BROOT_PATCH_VERSION 0)
set(R3BROOT_VERSION "${FAIRROOT_MAJOR_VERSION}.${FAIRROOT_MINOR_VERSION}.${FAIRROOT_PATCH_VERSION}")

if(NOT DEFINED ROOT_FOUND_VERSION)
    set(ROOT_FOUND_VERSION "${found_vers}")
endif()
message("ROOT_FOUND_VERSION = ${ROOT_FOUND_VERSION}")

set(FAIRROOT_LIBRARY_PROPERTIES
    ${FAIRROOT_LIBRARY_PROPERTIES}
    VERSION
    "${FAIRROOT_VERSION}"
    SOVERSION
    "${FAIRROOT_MAJOR_VERSION}")

string(COMPARE EQUAL
               "${CMAKE_GENERATOR}"
               "Xcode"
               IS_XCODE)
set(R3BLIBDIR ${CMAKE_BINARY_DIR}/lib)
if(${IS_XCODE})
    set(LD_LIBRARY_PATH ${R3BLIBDIR}/Debug ${R3BLIBDIR} ${LD_LIBRARY_PATH})
else(${IS_XCODE})
    set(LD_LIBRARY_PATH ${R3BLIBDIR} ${LD_LIBRARY_PATH})
endif(${IS_XCODE})

find_package(ucesb)
if(ucesb_FOUND)
    set(WITH_UCESB true)
    set(LD_LIBRARY_PATH ${ucesb_LIBRARY_DIR} ${LD_LIBRARY_PATH})
endif(ucesb_FOUND)

find_package(Garfield)

r3b_generate_version_info()

# Recurse into the given subdirectories.  This does not actually cause another cmake executable to
# run.  The same process will walk through the project's entire directory structure.

if(NOT FAIRROOT_FOUND)
    add_subdirectory(fairtools)
    add_subdirectory(base)
    add_subdirectory(dbase)
    add_subdirectory(geobase)
    add_subdirectory(parbase)
    add_subdirectory(generators)
    add_subdirectory(geane)
    add_subdirectory(trackbase)
    add_subdirectory(MbsAPI)
endif(NOT FAIRROOT_FOUND)

if(MODULE)
    add_subdirectory(${MODULE})
endif(MODULE)

if(NOT MODULE)
    add_subdirectory(r3bbase)
    if(WITH_UCESB)
        add_subdirectory(r3bsource)
    endif(WITH_UCESB)
    add_subdirectory(passive)
    add_subdirectory(califa)
    add_subdirectory(xball)
    add_subdirectory(dch)
    add_subdirectory(gfi)
    add_subdirectory(fiber)
    add_subdirectory(fi0)
    add_subdirectory(fi1a)
    add_subdirectory(fi1b)
    add_subdirectory(fi2a)
    add_subdirectory(fi2b)
    add_subdirectory(fi3a)
    add_subdirectory(fi3b)
    add_subdirectory(fi4)
    add_subdirectory(fi5)
    add_subdirectory(fi6)
    add_subdirectory(fi7)
    add_subdirectory(fi8)
    add_subdirectory(fi9)
    add_subdirectory(fi10)
    add_subdirectory(fi11)
    add_subdirectory(sfi)
    add_subdirectory(land)
    add_subdirectory(neuland)
    add_subdirectory(los)
    add_subdirectory(sci8)
    add_subdirectory(mtof)
    add_subdirectory(dtof)
    add_subdirectory(tof)
    add_subdirectory(tracker)
    add_subdirectory(startrack)
    add_subdirectory(strawtubes)
    add_subdirectory(r3bdata)
    add_subdirectory(field)
    add_subdirectory(macros)
    add_subdirectory(r3bgen)
    add_subdirectory(lumon)
    add_subdirectory(evtvis)
    add_subdirectory(mfi)
    add_subdirectory(psp)
    add_subdirectory(timestitcher)
    add_subdirectory(compilehelper)
    add_subdirectory(tcal)
    add_subdirectory(tracking)
    if(GARFIELD_FOUND)
        add_subdirectory(actar)
        set(LD_LIBRARY_PATH ${GARFIELD_LIBRARY_DIR} ${LD_LIBRARY_PATH})
    endif(GARFIELD_FOUND)

    if(EXISTS "${R3BROOT_SOURCE_DIR}/sofia/")
        message("Building SOFIA framework...")
        set(WITH_SOFIA true)
        add_definitions(-DSOFIA)
        add_subdirectory(sofia)
    endif(EXISTS "${R3BROOT_SOURCE_DIR}/sofia/")

    if(EXISTS "${R3BROOT_SOURCE_DIR}/glad-tpc/")
        message("Building GLAD-TPC detector...")
        set(WITH_GTPC true)
        add_definitions(-DGTPC)
        add_subdirectory(glad-tpc)
    endif(EXISTS "${R3BROOT_SOURCE_DIR}/glad-tpc/")

    if(EXISTS "${R3BROOT_SOURCE_DIR}/frs/")
        message("Building FRS framework...")
        set(WITH_FRS true)
        add_definitions(-DFRS)
        add_subdirectory(frs)
    endif(EXISTS "${R3BROOT_SOURCE_DIR}/frs/")

endif(NOT MODULE)

execute_process(COMMAND ${ROOT_CONFIG_EXECUTABLE} --has-opengl
                OUTPUT_VARIABLE EveIsBuild
                OUTPUT_STRIP_TRAILING_WHITESPACE)

if(${EveIsBuild} STREQUAL yes)
    message("Build the eventdisplay.")
    if(NOT FAIRROOT_FOUND)
        add_subdirectory(eventdisplay)
    endif(NOT FAIRROOT_FOUND)
else(${EveIsBuild} STREQUAL yes)
    message("The eventdisplay is not build, there is no OpenGL support.")
endif(${EveIsBuild} STREQUAL yes)

option(BUILD_DOXYGEN "Build Doxygen" OFF)
if(BUILD_DOXYGEN)
    message(STATUS "*** Building the Doxygen documentaion ***")
    add_subdirectory(doxygen)
endif(BUILD_DOXYGEN)

if(RULE_CHECKER_FOUND)

    add_custom_target(RULES
                      COMMAND ${RULE_CHECKER_SCRIPT1} ${CMAKE_BINARY_DIR} viol > violations.html
                      DEPENDS $ENV{ALL_RULES})

endif(RULE_CHECKER_FOUND)

if(NOT R3BROOT_MINIMAL)
    write_config_file(config.sh)
    write_config_file(config.csh)
else(NOT R3BROOT_MINIMAL)
    write_minimal_config_file(config.sh)
endif(NOT R3BROOT_MINIMAL)

write_env_cache(env_cache.sh)
