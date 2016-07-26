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

message(STATUS "Looking for ucesb...")

set(ucesb_LIBRARY_SHARED ext_data_clnt)

set(ucesb_LIBRARY_STATIC ext_data_clnt.o)

find_path(ucesb_INCLUDE_DIR NAMES ext_data_clnt.hh ext_h101.h
  PATHS $ENV{UCESB_DIR}
  	$ENV{UCESB_DIR}/hbook
  NO_DEFAULT_PATH
  DOC   "Path to ucesb include header files."
)

find_path(ucesb_LIBRARY_DIR NAMES "lib${ucesb_LIBRARY_SHARED}.so"
  PATHS $ENV{UCESB_DIR}
  	$ENV{UCESB_DIR}/hbook
  NO_DEFAULT_PATH
  DOC   "Path to ucesb library files."
)
#find_library(ucesb_LIBRARY_SHARED NAMES ${ucesb_LIBRARY_SHARED}
#  PATHS $ENV{UCESB_DIR}/hbook
#  NO_DEFAULT_PATH
#  DOC   "Path to ${LIBucesb_SHARED}."
#)

find_library(ucesb_LIBRARY_STATIC NAMES ${ucesb_LIBRARY_STATIC}
  PATHS $ENV{UCESB_DIR}/hbook
  NO_DEFAULT_PATH
  DOC   "Path to ${LIBucesb_STATIC}."
)

if(ucesb_INCLUDE_DIR
	AND ucesb_LIBRARY_SHARED
	AND ucesb_LIBRARY_STATIC
	AND ucesb_LIBRARY_DIR
)
	set(ucesb_FOUND true)
else(ucesb_INCLUDE_DIR
	AND ucesb_LIBRARY_SHARED
	AND ucesb_LIBRARY_STATIC
	AND ucesb_LIBRARY_DIR
)
	set(ucesb_FOUND false)
endif(ucesb_INCLUDE_DIR
	AND ucesb_LIBRARY_SHARED
	AND ucesb_LIBRARY_STATIC
	AND ucesb_LIBRARY_DIR
)

message("    - UCESB_DIR = $ENV{UCESB_DIR}")
message("    - ucesb_INCLUDE_DIR = ${ucesb_INCLUDE_DIR}")
message("    - ucesb_LIBRARY_SHARED = ${ucesb_LIBRARY_SHARED}")
message("    - ucesb_LIBRARY_STATIC = ${ucesb_LIBRARY_STATIC}")
message("    - ucesb_LIBRARY_DIR = ${ucesb_LIBRARY_DIR}")

if(ucesb_FOUND)
  add_definitions(-DWITH_UCESB)
  set(ucesb_LIBRARIES "${ucesb_LIBRARY_STATIC};${ucesb_LIBRARY_SHARED}")
  if(NOT ucesb_FIND_QUIETLY)
    	set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${ucesb_LIBRARY_DIR})
  endif(NOT ucesb_FIND_QUIETLY)
  message("-- Looking for ucesb... - found ucesb")
else(ucesb_FOUND)
  if(NOT ucesb_FIND_QUIETLY)
    if(ucesb_FIND_REQUIRED)
      message(FATAL_ERROR "Looking for ucesb... - Not found")
    else(ucesb_FIND_REQUIRED)
      message(STATUS "Looking for ucesb... - Not found")
    endif(ucesb_FIND_REQUIRED)
  endif(NOT ucesb_FIND_QUIETLY)
endif(ucesb_FOUND)

mark_as_advanced(
	ucesb_INCLUDE_DIR
	ucesb_LIBRARIES
	ucesb_LIBRARY_SHARED
	ucesb_LIBRARY_STATIC
)
