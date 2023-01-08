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

set(SYSTEM_INCLUDE_DIRECTORIES
    ${SYSTEM_INCLUDE_DIRECTORIES}
    ${BASE_INCLUDE_DIRECTORIES}
    ${EPICS_INCLUDE_DIR}
    ${EPICS_OS_INCLUDE_DIR})

set(INCLUDE_DIRECTORIES ${R3BROOT_SOURCE_DIR}/epics)

set(LINK_DIRECTORIES ${ROOT_LIBRARY_DIR} ${FAIRROOT_LIBRARY_DIR} ${EPICS_LIBRARY_DIR})

include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

set(SRCS R3BChannelAccessEPICS.cxx)

change_file_extension(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF ChannelAccessLinkDef.h)
set(LIBRARY_NAME R3BChannelAccess)
set(DEPENDENCIES Base ca)

generate_library()
