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

set(LIBRARY_NAME R3BNeulandReconstruction)
set(LINKDEF NeulandReconstructionLinkDef.h)

# set(DEPENDENCIES R3BNeulandShared R3BData PyROOT)
set(DEPENDENCIES R3BNeulandShared R3BData)

set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES} ${R3BROOT_SOURCE_DIR}/neuland/reconstruction
    ${R3BROOT_SOURCE_DIR}/neuland/reconstruction/multiplicity
    ${R3BROOT_SOURCE_DIR}/neuland/reconstruction/neutrons)
include_directories(${INCLUDE_DIRECTORIES})
# link_libraries(/home/jmayer/.pyenv/versions/3.7.6/lib/libpython3.7m.so.1.0)

set(SRCS
    multiplicity/R3BNeulandMultiplicityBayes.cxx
    multiplicity/R3BNeulandMultiplicityBayesPar.cxx
    multiplicity/R3BNeulandMultiplicityBayesTrain.cxx
    multiplicity/R3BNeulandMultiplicityCalorimetric.cxx
    multiplicity/R3BNeulandMultiplicityCalorimetricPar.cxx
    multiplicity/R3BNeulandMultiplicityCalorimetricTrain.cxx
    multiplicity/R3BNeulandMultiplicityCheat.cxx
    multiplicity/R3BNeulandMultiplicityFixed.cxx
    # multiplicity/R3BNeulandMultiplicityScikit.cxx
    neutrons/R3BNeulandNeutronsCheat.cxx
    neutrons/R3BNeulandNeutronsRValue.cxx
    # neutrons/R3BNeulandNeutronsScikit.cxx
    # neutrons/R3BNeulandNeutronsKeras.cxx
    R3BNeulandReconstructionContFact.cxx
    R3BNeulandNeutronReconstructionMon.cxx
    R3BNeulandNeutronReconstructionStatistics.cxx)
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

generate_library()
