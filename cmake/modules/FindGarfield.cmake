 ################################################################################
 #    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    #
 #                                                                              #
 #              This software is distributed under the terms of the             # 
 #         GNU Lesser General Public Licence version 3 (LGPL) version 3,        #  
 #                  copied verbatim in the file "LICENSE"                       #
 ################################################################################

 ################################################################
 # For this script to succeed, you need to install ucesb:
 # Check out the CVS repository and compile:
 #  export CVS_RSH="ssh"
 #  cvs -d :ext:land@lxgs08.gsi.de:/u/johansso/CVS co unpacker
 #  cvs -d :ext:land@lxgs08.gsi.de:/u/landcvs/CVS co unpackexp
 #  cd unpacker
 #  make -j empty
 # Make sure to use the same ROOT version as for R3BROOT
 # After installation export an environment variable UCESB_DIR
 # which points to the 'unpacker' directory you have checked out.
 # More information: http://fy.chalmers.se/~f96hajo/ucesb/
 ################################################################

message(STATUS "Looking for Garfield++...")

find_path(GARFIELD_INCLUDE_DIR NAMES Sensor.hh
  PATHS $ENV{GARFIELD_HOME}/Include
  NO_DEFAULT_PATH
  DOC   "Path to Garfield include header files."
)

find_path(GARFIELD_LIBRARY_DIR NAMES libGarfield${CMAKE_SHARED_LIBRARY_SUFFIX}
  PATHS $ENV{GARFIELD_HOME}/install/lib
  NO_DEFAULT_PATH
  DOC   "Path to Garfield library files."
)

if(GARFIELD_INCLUDE_DIR
   AND GARFIELD_LIBRARY_DIR
)
  set(GARFIELD_FOUND true)
else(GARFIELD_INCLUDE_DIR
     AND GARFIELD_LIBRARY_DIR
)
  set(GARFIELD_FOUND false)
endif(GARFIELD_INCLUDE_DIR
      AND GARFIELD_LIBRARY_DIR
)

message("    - GARFIELD_INCLUDE_DIR = ${GARFIELD_INCLUDE_DIR}")
message("    - GARFIELD_LIBRARY_DIR = ${GARFIELD_LIBRARY_DIR}")

if(GARFIELD_FOUND)
  message(STATUS "Garfield++ was found.")
else(GARFIELD_FOUND)
  message(STATUS "Could not find package Garfield++")
endif(GARFIELD_FOUND)

