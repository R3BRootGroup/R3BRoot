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

set(LIBRARY_NAME R3BNeulandCalibration)
set(LINKDEF NeulandCalibrationLinkDef.h)

#set(CMAKE_CXX_STANDARD 14)

set(DEPENDENCIES R3BBase R3BData R3BTCal R3BNeulandShared)

set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES}
    ${R3BROOT_SOURCE_DIR}/neuland/calibration
    ${R3BROOT_SOURCE_DIR}/neuland/shared
    ${R3BROOT_SOURCE_DIR}/r3bdata/losData
    ${R3BROOT_SOURCE_DIR}/tcal)
include_directories(${INCLUDE_DIRECTORIES})

set(SRCS
    R3BNeulandMappedHist.cxx
    R3BNeulandMapped2Cal.cxx
    R3BNeulandMapped2CalPar.cxx
    R3BNeulandTacquilaMapped2Cal.cxx
    R3BNeulandTacquilaMapped2CalPar.cxx
    R3BNeulandTacquilaMapped2QCalPar.cxx
    R3BNeulandHitHist.cxx
    # R3BNeulandCalTest.cxx
    R3BNeulandHitPar.cxx
    R3BNeulandCosmicTracker.cxx
    R3BNeulandHitCalibrationEngine.cxx
    R3BNeulandHitCalibrationBar.cxx
    R3BNeulandTSyncer.cxx
    LSQR.cxx
    R3BNeulandCal2HitPar.cxx
    R3BNeulandParFact.cxx
    R3BNeulandCal2Hit.cxx
    R3BNeulandHitModulePar.cxx
    R3BNeulandQCalPar.cxx
    R3BNeulandQCalFiller.cxx
    R3BNeulandProvideTStart.cxx
    R3BNeulandMappingPar.cxx)
  change_file_extension(*.cxx *.h HEADERS "${SRCS}")
  
generate_library()
