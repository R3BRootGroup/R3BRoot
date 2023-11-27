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

project(R3BNeulandCalibrationShare CXX)

set(LIBRARY_NAME R3BNeulandCalibrationShare)
set(LINKDEF NeulandCalShareLinkDef.h)

set(SRCS # cmake-format: sortable
         R3BNeulandCalibrationTask.cxx R3BNeulandParSet.cxx)

set(INCLUDE_DIRECTORIES
    # cmake-format: sortable
    ${CMAKE_CURRENT_SOURCE_DIR} ${R3BROOT_SOURCE_DIR}/neuland/shared ${R3BROOT_SOURCE_DIR}/r3bbase)
set(DEPENDENCIES # cmake-format: sortable
                 R3BBase range-v3::range-v3 Microsoft.GSL::GSL R3BNeulandShared)

generate_library()

target_include_directories(R3BNeulandCalibrationShare PUBLIC .)
target_link_libraries(R3BNeuland INTERFACE R3BNeulandCalibrationShare)
