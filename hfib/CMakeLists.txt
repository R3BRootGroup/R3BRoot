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
	calibration/R3BHFIBMapped2CalPar.cxx
	calibration/R3BHFIBMapped2Cal.cxx
	calibration/R3BHFIBCal2HitPar.cxx
	calibration/R3BHFIBCal2Hit.cxx
	pars/R3BHFIBContFact.cxx
	pars/R3BHFIBTotCalPar.cxx
	online/R3BHFIBOnlineSpectra.cxx
    )
set(HEADERS
	calibration/R3BHFIBMapped2CalPar.h
	calibration/R3BHFIBMapped2Cal.h
	calibration/R3BHFIBCal2HitPar.h
	calibration/R3BHFIBCal2Hit.h
	pars/R3BHFIBContFact.h
	pars/R3BHFIBTotCalPar.h
	online/R3BHFIBOnlineSpectra.h
    )
add_library_with_dictionary(
    LIBNAME
    R3BHFIB
    LINKDEF
    HFIBLinkDef.h
    HEADERS
    ${HEADERS}
    SRCS
    ${SRCS}
    INCLUDEDIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    calibration
    pars
    online
    DEPENDENCIES
    R3BTracking
    R3BTCal)


