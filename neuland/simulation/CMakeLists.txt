# cmake-format: off
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
# cmake-format: on

set(SRCS
    # cmake-format: sortable
    generator/CosmicMuon.cxx
    generator/R3BNeulandGeneratorFactory.cxx
    R3BNeuland.cxx
    R3BNeulandMCMon.cxx
    R3BNeulandPrimaryClusterFinder.cxx
    R3BNeulandPrimaryInteractionFinder.cxx
)

set(HEADERS
    # cmake-format: sortable
    generator/CosmicMuon.h
    generator/CosmicMuonDistributions.h
    generator/R3BNeulandGeneratorFactory.h
    R3BNeuland.h
    R3BNeulandMCMon.h
    R3BNeulandPrimaryClusterFinder.h
    R3BNeulandPrimaryInteractionFinder.h
)

add_library_with_dictionary(
    LIBNAME
    R3BNeulandSimulation
    LINKDEF
    NeulandSimulationLinkDef.h
    HEADERS
    ${HEADERS}
    SRCS
    ${SRCS}
    INCLUDEDIRS
    generator
    ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDENCIES
    R3BNeulandShared
    R3BPassive
    R3BNeulandGeoBase
    PRIVATE_DEPENDENCIES
    magic_enum::magic_enum
)
