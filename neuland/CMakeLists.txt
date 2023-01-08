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

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wformat-security -march=native")

set(SYSTEM_INCLUDE_DIRECTORIES ${SYSTEM_INCLUDE_DIRECTORIES} ${BASE_INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(INCLUDE_DIRECTORIES 
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/losData
${R3BROOT_SOURCE_DIR}/r3bdata/neulandData
${R3BROOT_SOURCE_DIR}/r3bbase 
${R3BROOT_SOURCE_DIR}/neuland/shared
)

link_directories(${ROOT_LIBRARY_DIR} ${Boost_LIBRARY_DIRS} ${FAIRROOT_LIBRARY_DIR})

add_subdirectory(calibration)
add_subdirectory(clustering)
add_subdirectory(digitizing)
add_subdirectory(online)
if(WITH_EPICS)
    add_subdirectory(preexp)
endif(WITH_EPICS)
add_subdirectory(reconstruction)
add_subdirectory(shared)
add_subdirectory(simulation)
add_subdirectory(test)
add_subdirectory(unpack)
add_subdirectory(executables)
