##############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2019-2023 Members of R3B Collaboration                     #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

# Create a library called "libR3BCalifa" which includes the source files given
# in the array. The extension is already found. Any number of sources could be
# listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
${Boost_INCLUDE_DIR}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/passive
${R3BROOT_SOURCE_DIR}/tracking
${R3BROOT_SOURCE_DIR}/califa
${R3BROOT_SOURCE_DIR}/califa/online
${R3BROOT_SOURCE_DIR}/califa/calibration
${R3BROOT_SOURCE_DIR}/califa/ana
${R3BROOT_SOURCE_DIR}/califa/pars
${R3BROOT_SOURCE_DIR}/califa/sim
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/wrData
${R3BROOT_SOURCE_DIR}/r3bdata/califaData
${R3BROOT_SOURCE_DIR}/r3bdata/amsData
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

file(GLOB SRCS 
./sim/*.cxx
./pars/*.cxx 
./online/*.cxx
./calibration/*.cxx
./ana/*.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF CalifaLinkDef.h)
set(LIBRARY_NAME R3BCalifa)

set(DEPENDENCIES
  Spectrum R3BBase R3BPassive R3BData R3BTracking Boost::regex)

GENERATE_LIBRARY()

if(BUILD_GEOMETRY)
  add_subdirectory(geobase)
endif()
add_subdirectory(test)
