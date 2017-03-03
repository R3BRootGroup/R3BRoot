# The name of our project is “R3BBROOT”.  CMakeLists files in this project can
# refer to the root source directory of the project as ${R3BROOT_SOURCE_DIR} or
# as ${CMAKE_SOURCE_DIR} and to the root binary directory of the project as
# ${R3BROOT_BINARY_DIR} or ${CMAKE_BINARY_DIR}.
# This difference is is important for the base classes which are in R3BROOT
# and PANDAROOT.

# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.2 FATAL_ERROR)

### CMP0025   Compiler id for Apple Clang is now AppleClang.
### CMP0042   MACOSX_RPATH is enabled by default.

foreach(p
  CMP0025 # CMake 3.0
  CMP0042 # CMake 3.0
  )
  if(POLICY ${p})
  cmake_policy(SET ${p} NEW)
  endif()
endforeach()

# Set name of our project to "R3BROOT". Has to be done
# after check of cmake version
project(R3BROOT)

# Check if the FairRoot is still in source, since Jan 2014 FairRoot
# should be installed separately

FIND_PATH(FAIRBASE NAMES FairRun.h  PATHS
  ${CMAKE_SOURCE_DIR}/base/steer
  NO_DEFAULT_PATH
)

If (FAIRBASE)
  Message(STATUS "Found FAIRBASE")
  SET(FAIRBASE ${FAIRBASE})
Else (FAIRBASE)
  Message(STATUS "NOT Found FAIRBASE")
  If(FAIRROOT_ROOT)
    Set(FAIRROOTPATH ${FAIRROOT_ROOT})
  Else(FAIRROOT_ROOT)
    IF(NOT DEFINED ENV{FAIRROOTPATH})
       MESSAGE(FATAL_ERROR "You did not define the environment variable FAIRROOTPATH which is needed to find FairRoot. Please set this variable and execute cmake again.")
    ENDIF(NOT DEFINED ENV{FAIRROOTPATH})
    SET(FAIRROOTPATH $ENV{FAIRROOTPATH})
  EndIf(FAIRROOT_ROOT)

EndIf (FAIRBASE)

#added a variable to point to r3broot path in the system
SET( R3BROOTPATH ${CMAKE_INSTALL_PREFIX} )

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/
# is checked


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules"  ${CMAKE_MODULE_PATH})
If(NOT FAIRROOT_ROOT)
  set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules_old"  ${CMAKE_MODULE_PATH})
EndIf(NOT FAIRROOT_ROOT)

# Check if the compiler support specific C++11 features
# Up to now this is only a check since the code does not use
# any of the features of the new standard

IF(FAIRROOTPATH)
  Set(CheckSrcDir "${FAIRROOTPATH}/share/fairbase/cmake/checks")
ELSE(FAIRROOTPATH)
  Set(CheckSrcDir "${CMAKE_SOURCE_DIR}/cmake/checks")
ENDIF(FAIRROOTPATH)

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


# R3BRoot uses from 10.08.15 C++11, so we need an compiler which supports C++11
# Check if the used compiler support C++11. If not stop with an error message
# Check also if FairSoft and FairRoot have been compiled with C++11 support
If(NOT _HAS_CXX11_FLAG)
  Message(FATAL_ERROR "The used C++ compiler (${CMAKE_CXX_COMPILER}) does not support C++11. R3BRoot can only be compiled with compilers supporting C++11. Please install such an compiler.")
EndIf()

If(NOT FAIRROOT_ROOT)
  Execute_process(COMMAND $ENV{SIMPATH}/bin/fairsoft-config --cxxflags OUTPUT_VARIABLE _res_fairsoft_config OUTPUT_STRIP_TRAILING_WHITESPACE)
  String(FIND ${_res_fairsoft_config} "-std=c++11" POS_C++11)
  String(FIND ${_res_fairsoft_config} "-std=c++1y" POS_C++1y)
  If(${POS_C++11} EQUAL -1 AND ${POS_C++1y} EQUAL -1)
    Message(FATAL_ERROR "FairSoft wasn't compiled with c++11 or c++1y support. Please recompile FairSoft with a compiler which supports c++11 or c++1y.")
  EndIf()

  Execute_process(COMMAND $ENV{FAIRROOTPATH}/bin/fairroot-config --fairsoft_path OUTPUT_VARIABLE _simpath OUTPUT_STRIP_TRAILING_WHITESPACE)
  Remove_Trailing_Slash($ENV{SIMPATH})
  Set(_simpath ${_ret_val})
  Remove_Trailing_Slash(${_simpath})
  Set(_fairroot_config ${_ret_val})
  String(COMPARE EQUAL ${_simpath} ${_fairroot_config}  _same_fairsoft)
  If(NOT _same_fairsoft)
      Message(STATUS "FairSoft version used for FairRoot compilation: ${_fairroot_config}")
      Message(STATUS "FairSoft version now (defined by SIMPATH): ${_simpath}")
      Message(FATAL_ERROR "Both versions must be the same.")
  EndIf()
EndIf(NOT FAIRROOT_ROOT)


#Check the compiler and set the compile and link flags
Check_Compiler()

Set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
Set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
Set(INCLUDE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/include")
Set(VMCWORKDIR ${CMAKE_SOURCE_DIR})

Option(USE_PATH_INFO "Information from PATH and LD_LIBRARY_PATH are
used." OFF)
If(USE_PATH_INFO)
  Set(PATH $ENV{PATH})
  If (APPLE)
    Set(LD_LIBRARY_PATH $ENV{DYLD_LIBRARY_PATH})
  Else (APPLE)
    Set(LD_LIBRARY_PATH $ENV{LD_LIBRARY_PATH})
  EndIf (APPLE)
EndIf(USE_PATH_INFO)

# Check if the user wants to build the project in the source
# directory
CHECK_OUT_OF_SOURCE_BUILD()

# Check if we are on an UNIX system. If not stop with an error
# message
IF(NOT UNIX)
  MESSAGE(FATAL_ERROR "You're not on an UNIX system. The project was up to now only tested on UNIX systems, so we break here. If you want to go on please edit the CMakeLists.txt in the source directory.")
ENDIF(NOT UNIX)

#Check if necessary environment variables are set
#If not stop execution
If(NOT FAIRROOT_ROOT)
  if(NOT R3BROOT_MINIMAL)
    IF(NOT DEFINED ENV{SIMPATH})
      MESSAGE(FATAL_ERROR "You did not define the environment variable SIMPATH which is nedded to find the external packages. Please set this variable and execute cmake again.")
    ENDIF(NOT DEFINED ENV{SIMPATH})
    STRING(REGEX MATCHALL "[^:]+" PATH $ENV{PATH})
    SET(SIMPATH $ENV{SIMPATH})
  else(NOT R3BROOT_MINIMAL)
    if(NOT DEFINED ENV{ROOTSYS})
      MESSAGE(FATAL_ERROR "You did not define the environment variable ROOTSYS which is nedded for the installation of the minimal version of FairRoot. Please set this variable and execute cmake again.")
    endif(NOT DEFINED ENV{ROOTSYS})
  endif(NOT R3BROOT_MINIMAL)
EndIf(NOT FAIRROOT_ROOT)

# Check if the external packages are installed into a separate install
# directory
CHECK_EXTERNAL_PACKAGE_INSTALL_DIR()


If(NOT FAIRROOT_ROOT)
  If(FAIRSOFT_EXTERN)
    set(BOOST_ROOT ${SIMPATH})
    set(GSL_DIR ${SIMPATH})
  Else(FAIRSOFT_EXTERN)
    set(BOOST_ROOT ${SIMPATH}/basics/boost)
    set(GSL_DIR ${SIMPATH}/basics/gsl)
  EndIf(FAIRSOFT_EXTERN)
EndIf(NOT FAIRROOT_ROOT)


Set(Boost_NO_SYSTEM_PATHS TRUE)
Set(Boost_NO_BOOST_CMAKE TRUE)
Message("-- Looking for Boost ...")
# If an older version of boost is found both of the variables below are
# cached and in a second cmake run, a good boost version is found even
# if the version is to old.
# To overcome this problem both variables are cleared before checking
# for boost.
Unset(Boost_INCLUDE_DIR CACHE)
Unset(Boost_LIBRARY_DIRS CACHE)
find_package(Boost 1.41 COMPONENTS thread system timer program_options)
If (Boost_FOUND)
  Set(Boost_Avail 1)
Else (Boost_FOUND)
  Set(Boost_Avail 0)
EndIf (Boost_FOUND)


SetBasicVariables()


# Check if FairRoot was compiled with FairDB option
find_library(FAIRDB_FOUND FairDB ${FAIRROOTPATH}/lib NO_DEFAULT_PATH)
if(FAIRDB_FOUND)
  Message("Found FairDB ...")
  Add_Definitions(-DWITH_FAIRDB)
  Set(DEFINITIONS "${DEFINITIONS} -DWITH_FAIRDB")
else(FAIRDB_FOUND)
  Message("Building without dbase ...")
endif(FAIRDB_FOUND)


# searches for needed packages
# REQUIRED means that cmake will stop if this packages are not found
# For example the framework can run without GEANT4, but ROOT is
# mandatory
if(R3BROOT_MINIMAL)
  MESSAGE("******* Install minimal FairRoot Version ********")
  find_package(ROOT 6 REQUIRED)
else(R3BROOT_MINIMAL)
  find_package(ROOT 6 REQUIRED)
#  find_package(PLUTO REQUIRED)
#  find_package(GENERATORS REQUIRED)
  If(FAIRROOT_ROOT)

    Message(STATUS "Looking for GEANT3...")
    find_package(Geant3 REQUIRED)
    If(Geant3_DIR)
      Set(GEANT3_INCLUDE_DIR ${Geant3_INCLUDE_DIRS})
      Set(GEANT3_LIBRARIES ${Geant3_LIBRARIES})
      Set(GEANT3_LIBRARY_DIR ${Geant3_DIR}/..)
      SET(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${GEANT3_LIBRARY_DIR})
      FIND_PATH(GEANT3_SYSTEM_DIR NAMES data PATHS
                ${Geant3_DIR}/../../share/geant3
                NO_DEFAULT_PATH
               )
      Set(GEANT3_FOUND TRUE)
      Message(STATUS "Looking for GEANT3... - found ${GEANT3_LIBRARIES}")
    EndIf(Geant3_DIR)

    Message(STATUS "Looking for GEANT4...")
    find_package(Geant4 REQUIRED)
    If(Geant4_DIR)
      Set(GEANT4_INCLUDE_DIR ${Geant4_INCLUDE_DIRS})
      Set(GEANT4_LIBRARY_DIR "${Geant4_DIR}/..")
      Set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${GEANT4_LIBRARY_DIR})
      Set(GEANT4_SHARE_DIR "${Geant4_DIR}/../../share")
      Set(GEANT4_FOUND TRUE)
      Message(STATUS "Looking for GEANT4... - found ${GEANT4_LIBRARY_DIR}")
      Set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/cmake/modules_old" ${CMAKE_MODULE_PATH})
      find_package(GEANT4DATA)
    EndIf(Geant4_DIR)

    Message(STATUS "Looking for GEANT4VMC...")
    find_package(Geant4VMC REQUIRED)
    If(Geant4VMC_DIR)
      Set(GEANT4VMC_INCLUDE_DIR ${Geant4VMC_INCLUDE_DIRS})
      Set(GEANT4VMC_LIBRARY_DIR "${Geant4VMC_DIR}/..")
      Set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${GEANT4VMC_LIBRARY_DIR})
      Set(GEANT4VMC_SHARE_DIR "${Geant4VMC_DIR}/../../share")
      Find_Path(GEANT4VMC_MACRO_DIR NAMES g4libs.C PATHS
                ${GEANT4VMC_SHARE_DIR}/Geant4VMC-3.3.0/examples/macro
                NO_DEFAULT_PATH
               )
      Set(GEANT4VMC_FOUND TRUE)
      Message(STATUS "Looking for GEANT4VMC... - found ${GEANT4VMC_LIBRARY_DIR}")
    EndIf(Geant4VMC_DIR)

    Message(STATUS "Looking for VGM...")
    Find_Package(VGM)
    If(VGM_DIR)
      Set(VGM_INCLUDE_DIR ${VGM_INCLUDE_DIRS})
      Set(VGM_LIBRARY_DIR "${VGM_DIR}/..")
      Set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${VGM_LIBRARY_DIR})
      Set(VGM_FOUND TRUE)
      Message(STATUS "Looking for VGM... - found ${VGM_LIBRARY_DIR}")
    EndIf()
#    find_package(CLHEP REQUIRED)
  Else(FAIRROOT_ROOT)
    find_package(GEANT3 REQUIRED)
    find_package(GEANT4 REQUIRED)
    find_package(GEANT4DATA)
    find_package(GEANT4VMC REQUIRED)
    find_package(CLHEP REQUIRED)
  EndIf(FAIRROOT_ROOT)
#  find_package(RuleChecker)
endif(R3BROOT_MINIMAL)

# set a variable which should be used in all CMakeLists.txt
# Defines all basic include directories from fairbase
SetBasicVariables()

# Set the library version in the main CMakeLists.txt
SET(R3BROOT_MAJOR_VERSION 0)
SET(R3BROOT_MINOR_VERSION 0)
SET(R3BROOT_PATCH_VERSION 0)
SET(R3BROOT_VERSION "${FAIRROOT_MAJOR_VERSION}.${FAIRROOT_MINOR_VERSION}.${FAIRROOT_PATCH_VERSION}")

If(NOT DEFINED ROOT_FOUND_VERSION)
  SET(ROOT_FOUND_VERSION "${found_vers}")
EndIf()
MESSAGE("ROOT_FOUND_VERSION = ${ROOT_FOUND_VERSION}")

SET(FAIRROOT_LIBRARY_PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES}
   VERSION "${FAIRROOT_VERSION}"
   SOVERSION "${FAIRROOT_MAJOR_VERSION}"
)

string(COMPARE EQUAL "${CMAKE_GENERATOR}" "Xcode" IS_XCODE)
SET(R3BLIBDIR ${CMAKE_BINARY_DIR}/lib)
If(${IS_XCODE})
  SET(LD_LIBRARY_PATH  ${R3BLIBDIR}/Debug ${R3BLIBDIR} ${LD_LIBRARY_PATH})
Else(${IS_XCODE})
  SET(LD_LIBRARY_PATH  ${R3BLIBDIR} ${LD_LIBRARY_PATH})
Endif(${IS_XCODE})

find_package(ucesb)
if(ucesb_FOUND)
set(WITH_UCESB true)
set(LD_LIBRARY_PATH ${ucesb_LIBRARY_DIR} ${LD_LIBRARY_PATH})
endif(ucesb_FOUND)

find_package(Garfield)

R3B_Generate_Version_Info()

# Recurse into the given subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.

IF(NOT FAIRROOT_FOUND)
add_subdirectory (fairtools)
add_subdirectory (base)
add_subdirectory (dbase)
add_subdirectory (geobase)
add_subdirectory (parbase)
add_subdirectory (generators)
add_subdirectory (geane)
add_subdirectory (trackbase)
add_subdirectory (MbsAPI)
ENDIF (NOT FAIRROOT_FOUND)

IF(MODULE)
add_subdirectory (${MODULE})
ENDIF (MODULE)

IF(NOT MODULE)
add_subdirectory (r3bbase)
if (WITH_UCESB)
  add_subdirectory (r3bsource)
endif (WITH_UCESB)
add_subdirectory (passive)
add_subdirectory (califa)
add_subdirectory (xball)
add_subdirectory (dch)
add_subdirectory (gfi)
add_subdirectory (fiber)
add_subdirectory (fi1)
add_subdirectory (fi4)
add_subdirectory (fi5)
add_subdirectory (fi6)
add_subdirectory (sfi)
add_subdirectory (land)
add_subdirectory (neuland)
add_subdirectory (los)
add_subdirectory (mtof)
add_subdirectory (dtof)
add_subdirectory (tof)
add_subdirectory (tracker)
add_subdirectory (startrack)
add_subdirectory (strawtubes)
add_subdirectory (r3bdata)
add_subdirectory (field)
add_subdirectory (macros)
add_subdirectory (r3bgen)
add_subdirectory (lumon)
add_subdirectory (evtvis)
add_subdirectory (mfi)
add_subdirectory (psp)
add_subdirectory (timestitcher)
add_subdirectory (compilehelper)
add_subdirectory (tcal)
add_subdirectory (tracking)
  IF(GARFIELD_FOUND)
    add_subdirectory (actar)
    Set(LD_LIBRARY_PATH ${GARFIELD_LIBRARY_DIR} ${LD_LIBRARY_PATH})
  ENDIF(GARFIELD_FOUND)

IF(EXISTS "${R3BROOT_SOURCE_DIR}/sofia/")
  Message("Building SOFIA framework...")
  Set(WITH_SOFIA true)
  add_definitions(-DSOFIA)
  add_subdirectory(sofia)
ENDIF(EXISTS "${R3BROOT_SOURCE_DIR}/sofia/")

IF(EXISTS "${R3BROOT_SOURCE_DIR}/glad-tpc/")
  Message("Building GLAD-TPC detector...")
  Set(WITH_GTPC true)
  add_definitions(-DGTPC)
  add_subdirectory(glad-tpc)
ENDIF(EXISTS "${R3BROOT_SOURCE_DIR}/glad-tpc/")

ENDIF (NOT MODULE)

Execute_Process(COMMAND ${ROOT_CONFIG_EXECUTABLE} --has-opengl
                OUTPUT_VARIABLE EveIsBuild
                OUTPUT_STRIP_TRAILING_WHITESPACE
               )

If(${EveIsBuild} STREQUAL yes)
  Message("Build the eventdisplay.")
IF(NOT FAIRROOT_FOUND)
  add_subdirectory (eventdisplay)
ENDIF(NOT FAIRROOT_FOUND)
Else(${EveIsBuild} STREQUAL yes)
  Message("The eventdisplay is not build, there is no OpenGL support.")
EndIf(${EveIsBuild} STREQUAL yes)



Option(BUILD_DOXYGEN "Build Doxygen" OFF)
if(BUILD_DOXYGEN)
  MESSAGE(STATUS "*** Building the Doxygen documentaion ***")
  ADD_SUBDIRECTORY(doxygen)
endif(BUILD_DOXYGEN)


if(RULE_CHECKER_FOUND)

  ADD_CUSTOM_TARGET(RULES
     COMMAND ${RULE_CHECKER_SCRIPT1} ${CMAKE_BINARY_DIR} viol > violations.html
     DEPENDS $ENV{ALL_RULES})

endif(RULE_CHECKER_FOUND)

if(NOT R3BROOT_MINIMAL)
  WRITE_CONFIG_FILE(config.sh)
  WRITE_CONFIG_FILE(config.csh)
else(NOT R3BROOT_MINIMAL)
  WRITE_MINIMAL_CONFIG_FILE(config.sh)
endif(NOT R3BROOT_MINIMAL)

WRITE_ENV_CACHE(env_cache.sh)
