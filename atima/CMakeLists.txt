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

# Create a library called "libR3BAtima" which includes the source files
#given in # the array .  # The extension is already found.  Any number
#of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES 
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
${R3BROOT_SOURCE_DIR}/atima
)

include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

set(SRCS
R3BAtima.cxx
R3BAtimaCache.cxx
)

CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF AtimaLinkDef.h)
Set(LIBRARY_NAME R3BAtima)
Set(DEPENDENCIES Atima::Atima)

GENERATE_LIBRARY()

add_subdirectory(test)
