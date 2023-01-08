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

set(LIBRARY_NAME R3BNeulandPreexp)
set(LINKDEF NeulandPreexpLinkDef.h)

set(DEPENDENCIES R3BData R3BBase R3BChannelAccess)

set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES}
    ${EPICS_INCLUDE_DIR}
    ${EPICS_OS_INCLUDE_DIR}
    ${R3BROOT_SOURCE_DIR}/epics
    ${R3BROOT_SOURCE_DIR}/neuland/preexp)
include_directories(${INCLUDE_DIRECTORIES})

link_directories(${EPICS_LIBRARY_DIR})

set(SRCS R3BNeulandCheckMapping.cxx R3BNeulandGainMatching.cxx R3BNeulandTimeRes.cxx)
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

generate_library()
