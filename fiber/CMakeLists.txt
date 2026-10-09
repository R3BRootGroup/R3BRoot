##############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2019 Members of R3B Collaboration                          #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

# Create a library called "libR3BPSP" which includes the source files
#given in # the array .  # The extension is already found.  Any number
#of sources could be listed here.

set(INCLUDE_DIRECTORIES
${R3BROOT_SOURCE_DIR}/r3bbase 
${R3BROOT_SOURCE_DIR}/r3bdata/fibData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/fiber
${R3BROOT_SOURCE_DIR}/tcal
${R3BROOT_SOURCE_DIR}/r3bdata
)

link_directories(${FAIRROOT_LIBRARY_DIR} ${ROOT_LIBRARY_DIR})

set(SRCS
R3BBunchedFiberMapped2CalPar.cxx
R3BBunchedFiberMapped2Cal.cxx
R3BBunchedFiberCal2Hit.cxx
#R3BBunchedFiberCal2Hitfull.cxx
#R3BBunchedFiberCal2HitEngRun2019.cxx
R3BBunchedFiberCal2Hit_s494.cxx
R3BBunchedFiberHitModulePar.cxx
R3BBunchedFiberHitPar.cxx
R3BFiberContFact.cxx
R3BBunchedFiberSPMTTrigMapped2CalPar.cxx
R3BBunchedFiberSPMTTrigMapped2Cal.cxx
R3BBunchedFiberSPMTTrigDigitizerCal.cxx
R3BFiberMAPMTMapped2Cal.cxx
R3BFiberMAPMTMapped2CalPar.cxx
R3BFiberMAPMTCal2Hit.cxx
R3BFiberMAPMTHitPar.cxx
R3BFiberMAPMTHitModulePar.cxx
)

CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF BunchedFiberLinkDef.h)
Set(LIBRARY_NAME R3BBunchedFiber)
set(DEPENDENCIES
     R3BPassive R3BData R3BTCal)

add_library_with_dictionary(
    LIBNAME
    ${LIBRARY_NAME}
    LINKDEF
    ${LINKDEF}
    HEADERS
    ${HEADERS}
    SRCS
    ${SRCS}
    INCLUDEDIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${INCLUDE_DIRECTORIES}
    DEPENDENCIES
    ${DEPENDENCIES})
    
#GENERATE_LIBRARY()
