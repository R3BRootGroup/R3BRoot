##############################################################################
#   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2026 Members of R3B Collaboration                          #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

set(SRCS
    pars/R3BPWContFact.cxx
    calibration/R3BPWMapped2PreCalPar.cxx
    calibration/R3BPWMapped2PreCal.cxx
    calibration/R3BPWPreCal2CalPar.cxx
    calibration/R3BPWPreCal2Cal.cxx
    pars/R3BPWTotCalPar.cxx
    calibration/R3BPWCal2Hit.cxx
    calibration/R3BPWCal2HitPar.cxx
    pars/R3BPWHitPar.cxx
    online/R3BPWOnlineSpectra.cxx
    )
set(HEADERS
    pars/R3BPWContFact.h
    calibration/R3BPWMapped2PreCalPar.h
    calibration/R3BPWMapped2PreCal.h
    calibration/R3BPWPreCal2CalPar.h
    calibration/R3BPWPreCal2Cal.h
    pars/R3BPWTotCalPar.h
    calibration/R3BPWCal2Hit.h
    calibration/R3BPWCal2HitPar.h
    pars/R3BPWHitPar.h
    online/R3BPWOnlineSpectra.h
    )
add_library_with_dictionary(
    LIBNAME
    R3BPW
    LINKDEF
    PWLinkDef.h
    HEADERS
    ${HEADERS}
    SRCS
    ${SRCS}
    INCLUDEDIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    calibration
    online
    pars
    DEPENDENCIES
    R3BTracking
    R3BTCal)
