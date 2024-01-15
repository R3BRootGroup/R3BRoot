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
set(SRCS
    R3BDigitizingChannel.cxx
    R3BDigitizingPaddle.cxx
    R3BDigitizingTacQuila.cxx
    R3BDigitizingTamex.cxx
    R3BDigitizingPaddleNeuland.cxx
    R3BNeulandHitMon.cxx
    R3BNeulandDigitizer.cxx
    R3BDigitizingPaddleMock.h
    R3BDigitizingChannelMock.h)

set(HEADERS
    R3BDigitizingChannel.h
    R3BDigitizingChannelMock.h
    R3BDigitizingEngine.h
    R3BDigitizingPaddle.h
    R3BDigitizingPaddleMock.h
    R3BDigitizingPaddleNeuland.h
    R3BDigitizingTacQuila.h
    R3BDigitizingTamex.h
    R3BNeulandDigitizer.h
    R3BNeulandHitMon.h)

add_library(R3BNeulandDigitizing SHARED ${SRCS})
target_include_directories(R3BNeulandDigitizing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(R3BNeulandDigitizing PUBLIC R3BData R3BNeulandShared R3BNeulandCalibration)
root_generate_dictionary(R3BNeulandDigitizing_dict ${HEADERS} MODULE R3BNeulandDigitizing LINKDEF
                         NeulandDigitizingLinkDef.h)
