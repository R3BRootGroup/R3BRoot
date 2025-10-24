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

set(LIBRARY_NAME R3BNeulandCalToHit)
set(LINKDEF NeulandCalToHitLinkDef.h)

set(SRCS
    # cmake-format: sortable
    engine/R3BNeulandCal2HitHistAnalysis.cxx
    engine/R3BNeulandMilleCalDataProcessor.cxx
    engine/R3BNeulandMillepede.cxx
    engine/R3BNeulandMuonRecons.cxx
    engine/R3BNeulandTSyncAnalysis.cxx
    R3BNeulandCalMonitorTask.cxx
    R3BNeulandCalToHitPar.cxx
    R3BNeulandCalToHitParTask.cxx
    R3BNeulandCalToHitTask.cxx)

set(HEADERS
    # cmake-format: sortable
    engine/R3BNeulandCal2HitHistAnalysis.h
    engine/R3BNeulandCosmicEngine.h
    engine/R3BNeulandMuonRecons.h
    engine/R3BNeulandTSyncAnalysis.h
    R3BNeulandCalToHitPar.h
    R3BNeulandCalToHitParTask.h
    R3BNeulandCalToHitTask.h)

set(ROOT_HEADERS # cmake-format: sortable
                 engine/R3BNeulandMilleCalDataProcessor.h R3BNeulandCalToHitPar.h)

add_library(R3BNeulandCalToHit SHARED ${SRCS})
target_include_directories(R3BNeulandCalToHit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} engine)
target_link_libraries(
    R3BNeulandCalToHit
    PUBLIC R3BNeulandMapToCal R3BNeulandCalibrationLegacy R3BMille
    PRIVATE magic_enum::magic_enum)

root_generate_dictionary(
    R3BNeulandCalToHit_dict
    ${ROOT_HEADERS}
    MODULE
    R3BNeulandCalToHit
    LINKDEF
    NeulandCalToHitLinkDef.h)
