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

set(SRCS R3BNeulandTcalFill.cxx R3BNeulandTcal.cxx R3BNeulandPmt.cxx)
set(HEADERS R3BNeulandTcalFill.h R3BNeulandTcal.h R3BNeulandPmt.h)

add_library_with_dictionary(
    LIBNAME
    R3BNeulandUnpack
    LINKDEF
    NeulandUnpackLinkDef.h
    HEADERS
    ${HEADERS}
    SRCS
    ${SRCS}
    INCLUDEDIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDENCIES
    R3BNeulandShared R3BTCal
)
