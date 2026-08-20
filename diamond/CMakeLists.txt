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
	pars/R3BDMContFact.cxx
	calibration/R3BDMMapped2CalPar.cxx
	calibration/R3BDMMapped2Cal.cxx
	pars/R3BDMTotCalPar.cxx
	calibration/R3BDMCal2Hit.cxx
	calibration/R3BDMCal2HitPar.cxx
	online/R3BDMOnlineSpectra.cxx
    )
set(HEADERS
	pars/R3BDMContFact.h
	calibration/R3BDMMapped2CalPar.h
	calibration/R3BDMMapped2Cal.h
	pars/R3BDMTotCalPar.h
	calibration/R3BDMCal2Hit.h
	calibration/R3BDMCal2HitPar.h
	online/R3BDMOnlineSpectra.h
    )
add_library_with_dictionary(
    LIBNAME
    R3BDM
    LINKDEF
    DMLinkDef.h
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
