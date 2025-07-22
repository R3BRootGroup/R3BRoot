#############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2019-2025 Members of R3B Collaboration                     #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

# Create a library called "libR3BFrsSci" which includes the source files given 
# in the array. The extension is already found. Any number of sources could be
# listed here.

set(SRCS
    pars/R3BFrsSciContFact.cxx
    pars/R3BFrsSciTcalPar.cxx
    pars/R3BFrsSciCalPar.cxx
    calibration/R3BFrsSciMapped2Tcal.cxx
    calibration/R3BFrsSciMapped2TcalPar.cxx
    calibration/R3BFrsSciTcal2Cal.cxx
    online/R3BOnlineSpectraFrsSci.cxx)

set(HEADERS
    pars/R3BFrsSciContFact.h
    pars/R3BFrsSciTcalPar.h
    pars/R3BFrsSciCalPar.h
    calibration/R3BFrsSciMapped2Tcal.h
    calibration/R3BFrsSciMapped2TcalPar.h
    calibration/R3BFrsSciTcal2Cal.h
    online/R3BOnlineSpectraFrsSci.h)

add_library_with_dictionary(
    LIBNAME
    R3BFrsSci
    LINKDEF
    FrsSciLinkDef.h
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
    R3BData
    R3BTCal)
    
add_subdirectory(test)
