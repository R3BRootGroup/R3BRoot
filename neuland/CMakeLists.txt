# cmake-format: off
##############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2019-2025 Members of R3B Collaboration                     #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################
# cmake-format: on

find_package(magic_enum REQUIRED)

add_subdirectory(calibration)
add_subdirectory(clustering)
add_subdirectory(online)
if(WITH_EPICS)
    add_subdirectory(preexp)
endif(WITH_EPICS)
add_subdirectory(reconstruction)
add_subdirectory(shared)
add_subdirectory(simulation)
add_subdirectory(unpack)
add_subdirectory(geobase)
if(CMAKE_CXX_STANDARD GREATER_EQUAL 23)
    add_subdirectory(digitizing)
    add_subdirectory(test)
    add_subdirectory(application)
    add_subdirectory(executables)
endif()
