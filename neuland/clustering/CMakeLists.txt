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

set(LIBRARY_NAME R3BNeulandClustering)
set(LINKDEF NeulandClusteringLinkDef.h)

set(DEPENDENCIES R3BNeulandShared R3BData)

set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} ${R3BROOT_SOURCE_DIR}/neuland/clustering)
include_directories(${INCLUDE_DIRECTORIES})

set(SRCS R3BNeulandClusterMon.cxx R3BNeulandClusterFinder.cxx)
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

generate_library()
