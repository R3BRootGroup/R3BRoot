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
set(SRCS R3BNeulandOnlineSpectra.cxx R3BNeulandOnlineReconstruction.cxx)
set(HEADERS R3BNeulandOnlineReconstruction.h R3BNeulandOnlineSpectra.h)

if(CMAKE_CXX_STANDARD GREATER_EQUAL 23)
    list(
        APPEND SRCS
        R3BNeulandOnlineSpectra2.cxx
        canvases/R3BNeulandCalCanvas.cxx
        canvases/R3BNeulandCountRateCanvas.cxx
        canvases/R3BNeulandEventHeaderCanvas.cxx
        canvases/R3BNeulandHitCanvas.cxx
        canvases/R3BNeulandHitCosmicCanvas.cxx
        canvases/R3BNeulandHitXYCanvas.cxx
        canvases/R3BNeulandMappedCanvas.cxx
        canvases/R3BNeulandOnlineCanvas.cxx
        canvases/R3BNeulandTJumpCanvas.cxx
        canvases/R3BNeulandTimingCanvas.cxx
    )

    list(
        APPEND HEADERS
        R3BNeulandOnlineSpectra2.h
        canvases/R3BNeulandCalCanvas.h
        canvases/R3BNeulandCountRateCanvas.h
        canvases/R3BNeulandEventHeaderCanvas.h
        canvases/R3BNeulandHitCanvas.h
        canvases/R3BNeulandHitCosmicCanvas.h
        canvases/R3BNeulandHitXYCanvas.h
        canvases/R3BNeulandMappedCanvas.h
        canvases/R3BNeulandOnlineCanvas.h
        canvases/R3BNeulandTJumpCanvas.h
        canvases/R3BNeulandTimingCanvas.h
    )
endif()

add_library(R3BNeulandOnline SHARED ${SRCS})
target_include_directories(
    R3BNeulandOnline
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} canvases
)
target_link_libraries(
    R3BNeulandOnline
    PUBLIC R3BNeulandCalibration R3BSource #[[ R3BTCal R3BNeulandShared ]]
)
root_generate_dictionary(
    R3BNeulandOnline_dict
    ${HEADERS}
    MODULE
    R3BNeulandOnline
    LINKDEF
    NeulandOnlineLinkDef.h
)
