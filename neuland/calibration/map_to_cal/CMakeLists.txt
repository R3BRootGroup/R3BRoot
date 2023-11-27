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

project(NeulandMapToCal CXX)

set(LIBRARY_NAME R3BNeulandMapToCal)
set(LINKDEF NeulandMapToCalLinkDef.h)

set(SRCS
    # cmake-format: sortable
    R3BFTCalEngine.cxx
    R3BNeulandMapToCalPar.cxx
    R3BNeulandMapToCalParTask.cxx
    R3BNeulandMapToCalTask.cxx
    R3BTrigIDMappingFinder.cxx)

set(INCLUDE_DIRECTORIES
    # cmake-format: sortable
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${R3BROOT_SOURCE_DIR}/neuland/calibration/share
    ${R3BROOT_SOURCE_DIR}/neuland/shared
    ${R3BROOT_SOURCE_DIR}/r3bbase
    ${R3BROOT_SOURCE_DIR}/r3bdata/neulandData)

set(DEPENDENCIES
    # cmake-format: sortable
    nlohmann_json::nlohmann_json
    R3BBase
    R3BData
    R3BNeulandCalibrationShare
    R3BNeulandShared
    R3BTCal
    range-v3::range-v3)

generate_library()
target_include_directories(R3BNeulandMapToCal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(R3BNeuland INTERFACE R3BNeulandMapToCal)
