# The name of our project is "FairBase".  CMakeLists files in this project can
# refer to the root source directory of the project as ${R3BROOT_SOURCE_DIR} or
# as ${CMAKE_SOURCE_DIR} and to the root binary directory of the project as 
# ${R3BROOT_BINARY_DIR} or ${CMAKE_BINARY_DIR}.
# This difference is is important for the base classes which are in R3BROOT 
# and PANDAROOT.

# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.1 FATAL_ERROR)

# Set name of our project to "R3BROOT". Has to be done
# after check of cmake version                        
project(R3BROOT)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ 
# is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# Load some basic macros which are needed later on
include(FairMacros)
include(WriteConfigFile)
include(Dart)
include(CheckCompiler)

#Check the compiler and set the compile and link flags
Check_Compiler()

Set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
Set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
Set(INCLUDE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/include")
Set(VMCWORKDIR ${CMAKE_SOURCE_DIR})

Option(USE_PATH_INFO "Information from PATH and LD_LIBRARY_PATH are
used."OFF)
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

# Check if the external packages are installed into a separate install
# directory
CHECK_EXTERNAL_PACKAGE_INSTALL_DIR()

SetBasicVariables()

# searches for needed packages
# REQUIRED means that cmake will stop if this packages are not found
# For example the framework can run without GEANT4, but ROOT is
# mandatory
if(R3BROOT_MINIMAL)
  MESSAGE("******* Install minimal FairRoot Version ********")
  find_package(ROOT 5.32.00 REQUIRED)
else(R3BROOT_MINIMAL)
  find_package(ROOT 5.32.00 REQUIRED)
  find_package(PLUTO REQUIRED)
  find_package(GENERATORS REQUIRED)
  find_package(GEANT3 REQUIRED)
  find_package(GEANT4 REQUIRED)
  find_package(GEANT4DATA)
  find_package(GEANT4VMC REQUIRED)
  find_package(CLHEP REQUIRED)
  find_package(RuleChecker)
  find_package(Oracle)
endif(R3BROOT_MINIMAL)

# set a variable which should be used in all CMakeLists.txt
# Defines all basic include directories from fairbase
SetBasicVariables()

# Set the library version in the main CMakeLists.txt
SET(R3BROOT_MAJOR_VERSION 0)
SET(R3BROOT_MINOR_VERSION 0)
SET(R3BROOT_PATCH_VERSION 0)
SET(R3BROOT_VERSION "${FAIRROOT_MAJOR_VERSION}.${FAIRROOT_MINOR_VERSION}.${FAIRROOT_PATCH_VERSION}")
SET(FAIRROOT_LIBRARY_PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES}
    VERSION "${R3BROOT_VERSION}"
    SOVERSION "${R3BROOT_MAJOR_VERSION}"
    SUFFIX ".so"
)

SET(R3BLIBDIR ${CMAKE_BINARY_DIR}/lib)  
SET(LD_LIBRARY_PATH  ${R3BLIBDIR} ${LD_LIBRARY_PATH})


Generate_Version_Info()

# Recurse into the given subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.
add_subdirectory (fairtools)
add_subdirectory (base)
add_subdirectory (dbase)
add_subdirectory (geobase)
add_subdirectory (parbase) 
add_subdirectory (generators)
add_subdirectory (geane)
add_subdirectory (trackbase)
add_subdirectory (MbsAPI)
add_subdirectory (r3bbase)  
add_subdirectory (passive) 
add_subdirectory (mcstack) 
add_subdirectory (cal)
add_subdirectory (xball)
add_subdirectory (dch) 
add_subdirectory (gfi) 
add_subdirectory (land) 
add_subdirectory (mtof) 
add_subdirectory (tof)
add_subdirectory (tracker)
add_subdirectory (startracker)
add_subdirectory (r3bdata) 
add_subdirectory (field) 
add_subdirectory (macros)  
add_subdirectory (eventdisplay)  
add_subdirectory (r3bgen) 
add_subdirectory (plists)
add_subdirectory (lumon) 
add_subdirectory (evtvis)
add_subdirectory (unpack)
add_subdirectory (mfi)
add_subdirectory (psp)


Option(BUILD_DOXYGEN "Build Doxygen" OFF)
if(BUILD_DOXYGEN)
  MESSAGE(STATUS "*** Building the Doxygen documentaion ***")
  ADD_SUBDIRECTORY(doxygen)
endif(BUILD_DOXYGEN)

if(ORACLE_FOUND)  
  add_subdirectory (ora)
endif(ORACLE_FOUND)  

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
