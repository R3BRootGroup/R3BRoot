# cmake-format: off
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
# cmake-format: on

set(SRCS
    # cmake-format: sortable
    LSQR.cxx
    R3BNeulandCal2Hit.cxx
    R3BNeulandCal2HitPar.cxx
    R3BNeulandCosmicTracker.cxx
    R3BNeulandHitCalibrationBar.cxx
    R3BNeulandHitCalibrationEngine.cxx
    R3BNeulandHitHist.cxx
    # R3BNeulandCalTest.cxx
    R3BNeulandHitModulePar.cxx
    R3BNeulandHitPar.cxx
    R3BNeulandMapped2Cal.cxx
    R3BNeulandMapped2CalPar.cxx
    R3BNeulandMappedHist.cxx
    R3BNeulandMappingPar.cxx
    R3BNeulandParFact.cxx
    R3BNeulandProvideTStart.cxx
    R3BNeulandQCalFiller.cxx
    R3BNeulandQCalPar.cxx
    R3BNeulandTacquilaMapped2Cal.cxx
    R3BNeulandTacquilaMapped2CalPar.cxx
    R3BNeulandTacquilaMapped2QCalPar.cxx
    R3BNeulandTSyncer.cxx)

set(HEADERS
    # cmake-format: sortable
    LSQR.h
    R3BNeulandCal2Hit.h
    R3BNeulandCal2HitPar.h
    R3BNeulandCosmicTracker.h
    R3BNeulandHitCalibrationBar.h
    R3BNeulandHitCalibrationEngine.h
    R3BNeulandHitHist.h
    # R3BNeulandCalTest.h
    R3BNeulandHitModulePar.h
    R3BNeulandHitPar.h
    R3BNeulandMapped2Cal.h
    R3BNeulandMapped2CalPar.h
    R3BNeulandMappedHist.h
    R3BNeulandMappingPar.h
    R3BNeulandParFact.h
    R3BNeulandProvideTStart.h
    R3BNeulandQCalFiller.h
    R3BNeulandQCalPar.h
    R3BNeulandTacquilaMapped2Cal.h
    R3BNeulandTacquilaMapped2CalPar.h
    R3BNeulandTacquilaMapped2QCalPar.h
    R3BNeulandTSyncer.h)

add_library(R3BNeulandCalibrationLegacy SHARED ${SRCS})
target_include_directories(R3BNeulandCalibrationLegacy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(R3BNeulandCalibrationLegacy PUBLIC R3BData R3BTCal R3BNeulandShared)
root_generate_dictionary(
    R3BNeulandCalibrationLegacy_dict
    ${HEADERS}
    MODULE
    R3BNeulandCalibrationLegacy
    LINKDEF
    NeulandCalibrationLinkDef.h)
