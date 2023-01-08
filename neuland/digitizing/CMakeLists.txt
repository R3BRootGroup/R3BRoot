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

set(INCLUDE_DIRECTORIES 
${INCLUDE_DIRECTORIES} 
${R3BROOT_SOURCE_DIR}/neuland/calibration
${R3BROOT_SOURCE_DIR}/neuland/digitizing
)
include_directories(${INCLUDE_DIRECTORIES})

set(SRCS
DigitizingEngine.cxx
DigitizingTacQuila.cxx
DigitizingTamex.cxx
R3BNeulandHitMon.cxx
R3BNeulandDigitizer.cxx
)
    
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF NeulandDigitizingLinkDef.h)
set(LIBRARY_NAME R3BNeulandDigitizing)
set(DEPENDENCIES R3BNeulandShared R3BData R3BNeulandCalibration)

GENERATE_LIBRARY()
