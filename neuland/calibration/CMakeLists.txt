# cmake-format: off
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
# cmake-format: on

add_subdirectory(legacy)
add_subdirectory(mille)
add_subdirectory(share)
add_subdirectory(map_to_cal)
add_subdirectory(cal_to_hit)

add_library(R3BNeulandCalibration INTERFACE)
target_link_libraries(
    R3BNeulandCalibration
    INTERFACE R3BNeulandCalibrationShare
              R3BNeulandMapToCal
              R3BNeulandCalToHit
              R3BNeulandCalibrationLegacy
              R3BSource)
