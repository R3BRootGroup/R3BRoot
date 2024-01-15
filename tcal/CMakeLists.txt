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

set(SRCS R3BTCalModulePar.cxx R3BTCalPar.cxx R3BTCalContFact.cxx R3BTCalEngine.cxx)
set(HEADERS R3BTCalModulePar.h R3BTCalPar.h R3BTCalContFact.h R3BTCalEngine.h)

add_library(R3BTCal SHARED ${SRCS})
target_include_directories(R3BTCal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(R3BTCal PUBLIC R3BBase)
root_generate_dictionary(R3BTCal_dict ${HEADERS} MODULE R3BTCal LINKDEF TCalLinkDef.h)
