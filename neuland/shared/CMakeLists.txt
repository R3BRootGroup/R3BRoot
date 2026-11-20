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
    ElasticScattering.cxx
    IsElastic.cxx
    R3BNeulandContFact.cxx
    R3BNeulandGeoPar.cxx
    R3BNeulandSignalMatcher.cxx
    R3BNeulandVisualizer.cxx
    R3BProgramOptions.cxx
)

if(CMAKE_CXX_STANDARD GREATER_EQUAL 23)
    list(APPEND SRCS R3BHuberRegression.cxx)
endif()

set(HEADERS
    ClusteringEngine.h
    ElasticScattering.h
    Filterable.h
    Validated.h
    IsElastic.h
    R3BNeulandCommon.h
    R3BNeulandCommonFunc.h
    R3BNeulandContFact.h
    R3BNeulandGeoPar.h
    R3BNeulandSignalMatcher.h
    R3BNeulandVisualizer.h
    R3BProgramOptions.h
    Validated.h
)

add_library_with_dictionary(
    LIBNAME
    R3BNeulandShared
    LINKDEF
    NeulandSharedLinkDef.h
    HEADERS
    ${HEADERS}
    SRCS
    ${SRCS}
    INCLUDEDIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDENCIES
    R3BData
    Boost::program_options
    Microsoft.GSL::GSL
    range-v3::range-v3
    nlohmann_json::nlohmann_json
)
