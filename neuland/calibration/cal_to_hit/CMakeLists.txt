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

project(NeulandCalToHit CXX)

set(LIBRARY_NAME R3BNeulandCalToHit)
set(LINKDEF NeulandCalToHitLinkDef.h)

set(SRCS
    # cmake-format: sortable
    engine/R3BNeulandLSQREngineAdaptor.cxx
    R3BNeulandCalToHitTask.cxx
    R3BNeulandCalToHitParTask.cxx
    R3BNeulandCalToHitPar.cxx)

set(INCLUDE_DIRECTORIES
    # cmake-format: sortable
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${R3BROOT_SOURCE_DIR}/neuland/calibration/legacy
    ${R3BROOT_SOURCE_DIR}/neuland/calibration/map_to_cal
    ${R3BROOT_SOURCE_DIR}/neuland/calibration/share
    ${R3BROOT_SOURCE_DIR}/neuland/shared
    ${R3BROOT_SOURCE_DIR}/r3bbase
    ${R3BROOT_SOURCE_DIR}/r3bdata/neulandData)

set(DEPENDENCIES
    # cmake-format: sortable
    R3BBase
    R3BData
    R3BNeulandCalibrationLegacy
    R3BNeulandCalibrationShare
    R3BNeulandMapToCal
    R3BNeulandShared
    range-v3::range-v3)

generate_library()
target_include_directories(R3BNeulandCalToHit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} engine)
target_link_libraries(R3BNeuland INTERFACE R3BNeulandCalToHit)
