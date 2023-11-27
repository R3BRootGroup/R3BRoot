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

set(LIBRARY_NAME R3BNeulandCalibrationLegacy)
set(LINKDEF NeulandCalibrationLinkDef.h)

# set(CMAKE_CXX_STANDARD 14)

set(DEPENDENCIES
    # cmake-format: sortable
    R3BBase
    R3BData
    R3BNeulandShared
    R3BTCal)

set(INCLUDE_DIRECTORIES
    # cmake-format: sortable
    ${INCLUDE_DIRECTORIES}
    ${R3BROOT_SOURCE_DIR}/neuland/calibration/legacy
    ${R3BROOT_SOURCE_DIR}/neuland/shared
    ${R3BROOT_SOURCE_DIR}/r3bdata/losData
    ${R3BROOT_SOURCE_DIR}/tcal)
include_directories(${INCLUDE_DIRECTORIES})

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

change_file_extension(
    *.cxx
    *.h
    HEADERS
    "${SRCS}")

generate_library()
target_include_directories(R3BNeulandCalibrationLegacy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
