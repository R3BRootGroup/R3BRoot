##############################################################################
#   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2022 Members of R3B Collaboration                          #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

# Create a library called "libR3BGConfig" which includes the source files given in
# the array. The extension is already found. Any number of sources could be listed
# here.

Set(SYSTEM_INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${BASE_INCLUDE_DIRECTORIES}
${YAML_CPP_INCLUDE_DIR}
${R3BROOT_SOURCE_DIR}/gconfig
)


Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM  ${SYSTEM_INCLUDE_DIRECTORIES})

link_directories(${FAIRROOT_LIBRARY_DIR} ${ROOT_LIBRARY_DIR})

Set(SRCS
#  R3BVMCConfig.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF GConfigLinkDef.h)
Set(LIBRARY_NAME R3BGConfig)
Set(DEPENDENCIES Base FairTools)

GENERATE_LIBRARY()
