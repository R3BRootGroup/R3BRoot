##############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2019 Members of R3B Collaboration                          #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

set(LIBRARY_NAME R3BNeulandOnline)
set(LINKDEF NeulandOnlineLinkDef.h)

set(DEPENDENCIES R3BData R3BBase)

set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} 
${R3BROOT_SOURCE_DIR}/neuland/online)
include_directories(${INCLUDE_DIRECTORIES})

set(SRCS R3BNeulandOnlineReconstruction.cxx R3BNeulandOnlineSpectra.cxx)

set(HEADERS R3BNeulandOnlineReconstruction.h R3BNeulandOnlineSpectra.h)

generate_library()
