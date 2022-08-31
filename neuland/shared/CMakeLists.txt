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

set(LIBRARY_NAME R3BNeulandShared)
set(LINKDEF NeulandSharedLinkDef.h)

set(DEPENDENCIES R3BData R3BBase ParBase)

include_directories(${INCLUDE_DIRECTORIES})

set(SRCS
    ElasticScattering.cxx
    IsElastic.cxx
    R3BNeulandGeoPar.cxx
    R3BNeulandContFact.cxx
    R3BNeulandVisualizer.cxx)

set(HEADERS
    ClusteringEngine.h
    ElasticScattering.h
    Filterable.h
    TCAConnector.h
    Validated.h
    IsElastic.h
    R3BNeulandGeoPar.h
    R3BNeulandContFact.h
    R3BNeulandVisualizer.h
    R3BNeulandCommon.h)

generate_library()

# find_package(Protobuf REQUIRED) protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS neuland.proto)
# protobuf_generate_python(PROTO_PY neuland.proto) set(SRCS ${SRCS} ${PROTO_SRCS}) set(HEADERS
# ${HEADERS} ${PROTO_HDRS}) Include directory such that protoc generated headers are found
# include_directories(${CMAKE_CURRENT_BINARY_DIR}) ${PROTOBUF_LIBRARY}
