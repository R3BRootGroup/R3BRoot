##############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2019 Members of R3B Collaboration                          #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

# The name of our project is “R3BBROOT”.  CMakeLists files in this project can refer to the root
# source directory of the project as ${R3BROOT_SOURCE_DIR} or as ${CMAKE_SOURCE_DIR} and to the root
# binary directory of the project as ${R3BROOT_BINARY_DIR} or ${CMAKE_BINARY_DIR}. This difference
# is is important for the base classes which are in R3BROOT and PANDAROOT.

# Check if cmake has the required version
cmake_minimum_required(VERSION 3.11.0 FATAL_ERROR)

# Set name of our project to "R3BROOT". Has to be done after check of cmake version
project(R3BROOT C CXX Fortran)

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/macros")
    message(FATAL_ERROR
            "\nmacros repository is missing in the R3BRoot source directory\ncd ../R3BRoot\ngit clone https://github.com/R3BRootGroup/macros.git"
           )
endif(NOT EXISTS "${PROJECT_SOURCE_DIR}/macros")

# Check for needed environment variables
IF(NOT DEFINED ENV{FAIRROOTPATH})
  MESSAGE(FATAL_ERROR "You did not define the environment variable FAIRROOTPATH which is needed to find FairRoot. Please set this variable and execute cmake again.")
ENDIF(NOT DEFINED ENV{FAIRROOTPATH})
SET(FAIRROOTPATH $ENV{FAIRROOTPATH})

IF(NOT DEFINED ENV{SIMPATH})
  MESSAGE(FATAL_ERROR "You did not define the environment variable SIMPATH which is nedded to find the external packages. Please set this variable and execute cmake again.")
ENDIF(NOT DEFINED ENV{SIMPATH})
SET(SIMPATH $ENV{SIMPATH})

STRING(REGEX MATCHALL "[^:]+" PATH $ENV{PATH})

set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules_old" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

Set(CheckSrcDir "${FAIRROOTPATH}/share/fairbase/cmake/checks")

# set cmake policies
include(FairRootCmakePolicies)
set_fairroot_cmake_policies()

find_package(FairRoot)

execute_process(COMMAND ${FAIRROOTPATH}/bin/fairroot-config --major_version OUTPUT_VARIABLE FR_MAJOR_VERSION)
execute_process(COMMAND ${FAIRROOTPATH}/bin/fairroot-config --minor_version OUTPUT_VARIABLE FR_MINOR_VERSION)
execute_process(COMMAND ${FAIRROOTPATH}/bin/fairroot-config --patch_version OUTPUT_VARIABLE FR_PATCH_VERSION)
if(${FR_MAJOR_VERSION} LESS 18 OR ${FR_MINOR_VERSION} LESS 2 OR ${FR_PATCH_VERSION} LESS 1)
  Message(FATAL_ERROR "Incompatible FairRoot version detected. R3BRoot requires FairRoot v18.2.1")
endif()

# added a variable to point to r3broot path in the system
set(R3BROOTPATH ${CMAKE_INSTALL_PREFIX})

# Check if the compiler support specific C++11 features Up to now this is only a check since the
# code does not use any of the features of the new standard
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

if(SIMPATH)
  set(CMAKE_PREFIX_PATH ${SIMPATH} ${CMAKE_PREFIX_PATH})
endif()

find_package2(PUBLIC ROOT  VERSION 6.10.00  REQUIRED)
find_package2(PUBLIC FairLogger  VERSION 1.2.0 REQUIRED)
find_package2(PUBLIC Pythia6)
find_package2(PUBLIC Pythia8)

find_package2(PUBLIC Geant3)
if(Geant3_FOUND)
  Set(Geant3_LIBRARY_DIR ${Geant3_DIR}/${Geant3_CMAKE_INSTALL_LIBDIR})
  FIND_PATH(Geant3_SYSTEM_DIR NAMES data PATHS
      ${Geant3_DIR}/../../share/Geant3-${Geant3_VERSION}
           NO_DEFAULT_PATH)
endif()

find_package2(PUBLIC Geant4)
#find_package2(PUBLIC GEANT4DATA)

find_package2(PUBLIC VGM)

find_package2(PUBLIC Geant4VMC)
if(Geant4VMC_FOUND)
  Set(Geant4VMC_LIBRARY_DIR "${Geant4VMC_DIR}/${Geant4VMC_CMAKE_INSTALL_LIBDIR}")
  Set(Geant4VMC_SHARE_DIR "${Geant4VMC_DIR}/share")
  Find_Path(Geant4VMC_MACRO_DIR NAMES g4libs.C PATHS
    ${Geant4VMC_SHARE_DIR}/Geant4VMC-${Geant4VMC_VERSION}/examples/macro
    NO_DEFAULT_PATH
  )
Endif()

#find_package(CLHEP)
find_package(HEPMC)

# Check the compiler and set the compile and link flags
check_compiler()

#set(CMAKE_CXX_FLAGS -std=c++14)

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

# Check if the external packages are installed into a separate install directory
check_external_package_install_dir()

set(Boost_NO_SYSTEM_PATHS TRUE)
set(Boost_NO_BOOST_CMAKE TRUE)

If(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
  set(BOOST_ROOT ${SIMPATH})
  set(GSL_DIR ${SIMPATH})
Else(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
  set(BOOST_ROOT ${SIMPATH}/basics/boost)
  set(GSL_DIR ${SIMPATH}/basics/gsl)
EndIf(${ROOT_LIBRARY_DIR} MATCHES /lib/root)

unset(Boost_INCLUDE_DIR CACHE)
unset(Boost_LIBRARY_DIRS CACHE)
find_package2(PUBLIC Boost
  VERSION 1.67 ${FairMQ_Boost_VERSION}
  COMPONENTS thread system timer program_options random filesystem chrono exception 
regex serialization log log_setup atomic date_time signals ${FairMQ_Boost_COMPONENTS}
)
if(Boost_FOUND)
    set(Boost_Avail 1)
else(Boost_FOUND)
    set(Boost_Avail 0)
endif(Boost_FOUND)

#if(SIMPATH)
#  set(CMAKE_PREFIX_PATH ${SIMPATH} ${CMAKE_PREFIX_PATH})
#endif()

SetBasicVariables()

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
    set(LD_LIBRARY_PATH ${R3BLIBDIR}/RELWITHDEBINFO ${R3BLIBDIR} ${LD_LIBRARY_PATH})
else(${IS_XCODE})
    set(LD_LIBRARY_PATH ${R3BLIBDIR} ${LD_LIBRARY_PATH})
endif(${IS_XCODE})

find_package(ucesb)
if(ucesb_FOUND)
    set(WITH_UCESB true)
    set(LD_LIBRARY_PATH ${ucesb_LIBRARY_DIR} ${LD_LIBRARY_PATH})
endif(ucesb_FOUND)

find_package(EPICS)
if(EPICS_FOUND)
  set(WITH_EPICS true)
  set(LD_LIBRARY_PATH ${EPICS_LIBRARY_DIR} ${LD_LIBRARY_PATH})
endif(EPICS_FOUND)

find_package(Atima)

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
    if (WITH_EPICS)
      add_subdirectory (epics)
    endif (WITH_EPICS)
    if (Atima_FOUND)
      add_subdirectory (atima)
    endif (Atima_FOUND)
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
    add_subdirectory(fi12)
    add_subdirectory(fi13)
    add_subdirectory(sfi)
    add_subdirectory(land)
    add_subdirectory(neuland)
    add_subdirectory(los)
    add_subdirectory(rolu)
    add_subdirectory(sci2)
    add_subdirectory(sci8)
    add_subdirectory(mtof)
    add_subdirectory(dtof)
    add_subdirectory(tof)
    add_subdirectory(tracker)
    add_subdirectory(startrack)
    add_subdirectory(strawtubes)
    add_subdirectory(music)
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
    add_subdirectory(tracker_rene)
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

# Summary ######################################################################
if(CMAKE_CXX_FLAGS)
  message(STATUS "  ")
  message(STATUS "  ${Cyan}GLOBAL CXX FLAGS${CR}  ${BGreen}${CMAKE_CXX_FLAGS}${CR}")
endif()
if(CMAKE_CONFIGURATION_TYPES)
  message(STATUS "  ")
  message(STATUS "  ${Cyan}BUILD TYPE         CXX FLAGS${CR}")
  string(TOUPPER "${CMAKE_BUILD_TYPE}" selected_type)
  foreach(type IN LISTS CMAKE_CONFIGURATION_TYPES)
   string(TOUPPER "${type}" type_upper)
   if(type_upper STREQUAL selected_type)
     pad("${type}" 18 " " type_padded)
     message(STATUS "${BGreen}* ${type_padded}${CMAKE_CXX_FLAGS_${type_upper}}${CR}")
   else()
     pad("${type}" 18 " " type_padded)
     message(STATUS "  ${BWhite}${type_padded}${CR}${CMAKE_CXX_FLAGS_${type_upper}}")
   endif()
   unset(type_padded)
   unset(type_upper)
 endforeach()
message(STATUS "  ")
message(STATUS "  (Change the build type with ${BMagenta}-DCMAKE_BUILD_TYPE=...${CR})")
endif()

if(PROJECT_PACKAGE_DEPENDENCIES)
  message(STATUS "  ")
  message(STATUS "  ${Cyan}DEPENDENCY FOUND     VERSION                   PREFIX${CR}")
  foreach(dep IN LISTS PROJECT_PACKAGE_DEPENDENCIES)
    if(${dep}_VERSION)
      if(${dep} STREQUAL DDS)
        set(version_str "${BGreen}${${dep}_MAJOR_VERSION}.${${dep}_MINOR_VERSION}${CR}")
      elseif(${dep} STREQUAL Boost)
        if(Boost_VERSION_MAJOR)
          set(version_str "${BGreen}${${dep}_VERSION_MAJOR}.${${dep}_VERSION_MINOR}${CR}")
        else()
          set(version_str "${BGreen}${${dep}_MAJOR_VERSION}.${${dep}_MINOR_VERSION}${CR}")
        endif()
      else()
        set(version_str "${BGreen}${${dep}_VERSION}${CR}")
      endif()
    else()
      set(version_str "${BYellow}unknown${CR}")
    endif()
    if(PROJECT_${dep}_VERSION)
      set(version_req_str " (>= ${PROJECT_${dep}_VERSION})")
    endif()
    pad(${dep} 20 " " dep_padded)
    if(DISABLE_COLOR)
      pad("${version_str}${version_req_str}" 25 " " version_padded)
    else()
      pad("${version_str}${version_req_str}" 25 " " version_padded COLOR 1)
    endif()
    set(prefix ${${dep}_PREFIX})
    if(${dep} STREQUAL Boost)
      if(TARGET Boost::headers)
        get_target_property(boost_include Boost::headers INTERFACE_INCLUDE_DIRECTORIES)
      else()
        get_target_property(boost_include Boost::boost INTERFACE_INCLUDE_DIRECTORIES)
      endif()
      get_filename_component(prefix ${boost_include}/.. ABSOLUTE)
    elseif(${dep} STREQUAL Protobuf)
      get_filename_component(prefix ${Protobuf_INCLUDE_DIRS}/.. ABSOLUTE)
    elseif(${dep} STREQUAL msgpack)
      get_target_property(msgpack_include msgpackc INTERFACE_INCLUDE_DIRECTORIES)
      get_filename_component(prefix ${msgpack_include}/.. ABSOLUTE)
    elseif(${dep} STREQUAL Pythia6)
      get_filename_component(prefix ${PYTHIA6_LIBRARY_DIR}/.. ABSOLUTE)
    elseif(${dep} STREQUAL Pythia8)
      get_filename_component(prefix ${PYTHIA8_LIB_DIR}/.. ABSOLUTE)
    elseif(${dep} STREQUAL FairLogger)
      if(FairLogger_PREFIX)
        set(prefix ${FairLogger_PREFIX})
      else()
        set(prefix ${FairLogger_ROOT})
      endif()
    elseif(${dep} STREQUAL FairMQ)
      if(FairMQ_PREFIX)
        set(prefix ${FairMQ_PREFIX})
      else()
        set(prefix ${FairMQ_ROOT})
      endif()
    elseif(${dep} STREQUAL yaml-cpp)
      get_filename_component(prefix ${YAML_CPP_INCLUDE_DIR}/.. ABSOLUTE)
    elseif(${dep} STREQUAL Geant4VMC)
      string(REPLACE ":" ";" geant4vmc_include ${Geant4VMC_INCLUDE_DIRS})
      list(GET geant4vmc_include 0 geant4vmc_include)
      get_filename_component(prefix ${geant4vmc_include}/../.. ABSOLUTE)
    elseif(${dep} STREQUAL Geant3)
      string(REPLACE ":" ";" geant3_include ${Geant3_INCLUDE_DIRS})
      list(GET geant3_include 0 geant3_include)
      get_filename_component(prefix ${geant3_include}/../.. ABSOLUTE)
    elseif(${dep} STREQUAL Geant4)
      list(GET Geant4_INCLUDE_DIRS 0 geant4_include)
      get_filename_component(prefix ${geant4_include}/../.. ABSOLUTE)
    elseif(${dep} STREQUAL VGM)
      string(REPLACE ":" ";" vgm_include ${VGM_INCLUDE_DIRS})
      list(GET vgm_include 0 vgm_include)
      get_filename_component(prefix ${vgm_include}/.. ABSOLUTE)
    elseif(${dep} STREQUAL ROOT)
      set(prefix ${ROOT_INSTALL_DIR})
    elseif(${dep} STREQUAL IWYU)
      get_filename_component(prefix ${IWYU_BINARY}/.. ABSOLUTE)
    elseif(${dep} STREQUAL yaml-cpp)
      get_filename_component(prefix ${YAML_CPP_INCLUDE_DIR}/.. ABSOLUTE)
    endif()

    message(STATUS "  ${BWhite}${dep_padded}${CR}${version_padded}${prefix}")

    if(${dep} STREQUAL Geant3)
      message(STATUS "                                          G3SYS: ${Geant3_SYSTEM_DIR}")
    endif()
    if(${dep} STREQUAL Geant4)
      foreach(dataset IN LISTS Geant4_DATASETS)
        pad(${Geant4_DATASET_${dataset}_ENVVAR} 20 " " envvar_padded LEFT)
        message(STATUS "                          ${envvar_padded}: ${Geant4_DATASET_${dataset}_PATH}")
      endforeach()
    endif()

    unset(version_str)
    unset(version_padded)
    unset(version_req_str)
  endforeach()
endif()
message(STATUS "  ")

   ################################################################################


