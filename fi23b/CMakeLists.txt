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
${R3BROOT_SOURCE_DIR}/fi23b
${R3BROOT_SOURCE_DIR}/fiber
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/fibData
${R3BROOT_SOURCE_DIR}/tcal
${R3BROOT_SOURCE_DIR}/tracking
)

link_directories(${FAIRROOT_LIBRARY_DIR} ${ROOT_LIBRARY_DIR})

set(SRCS
R3BFi23bCal2Hit.cxx
R3BFi23bMapped2Cal.cxx
R3BFi23b.cxx  
R3BFi23bContFact.cxx
R3BFi23bDigitizer.cxx
R3BFi23bDigitizerCal.cxx
R3BFi23bDigitizerHit.cxx
)

CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF Fi23bLinkDef.h)
Set(LIBRARY_NAME R3BFi23b)
Set(DEPENDENCIES
    R3BBunchedFiber R3BTracking R3BData)

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
