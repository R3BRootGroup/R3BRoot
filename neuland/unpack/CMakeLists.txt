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

set(LIBRARY_NAME R3BNeulandUnpack)
set(LINKDEF NeulandUnpackLinkDef.h)

set(SRCS R3BNeulandTcalFill.cxx R3BNeulandTcal.cxx R3BNeulandPmt.cxx)
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

set(DEPENDENCIES R3BBase R3BTCal R3BData)

set(INCLUDE_DIRECTORIES
    ${R3BROOT_SOURCE_DIR}/neuland/unpack
    ${R3BROOT_SOURCE_DIR}/r3bbase/
    ${R3BROOT_SOURCE_DIR}/r3bdata/
    ${R3BROOT_SOURCE_DIR}/r3bdata/neulandData
    ${R3BROOT_SOURCE_DIR}/r3bbase
    ${R3BROOT_SOURCE_DIR}/tcal)
include_directories(${INCLUDE_DIRECTORIES})

generate_library()
