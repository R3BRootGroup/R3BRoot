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

# Create a library called "libR3BBase" which includes the source files given in
# the array. The extension is already found. Any number of sources could be
# listed here.

set(SYSTEM_INCLUDE_DIRECTORIES ${SYSTEM_INCLUDE_DIRECTORIES} ${BASE_INCLUDE_DIRECTORIES})

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/tcal
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/sampData
${R3BROOT_SOURCE_DIR}/r3bdata/wrData
${R3BROOT_SOURCE_DIR}/r3bbase/pars
)

include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

link_directories(${FAIRROOT_LIBRARY_DIR} ${ROOT_LIBRARY_DIR})

set(SRCS
    R3BCoarseTimeStitch.cxx
    R3BDataPropagator.cxx
    R3BDetector.cxx
    R3BEventHeader.cxx
    R3BEventHeaderPropagator.cxx
    R3BException.cxx
    R3BFileSource.cxx
    R3BFileSource2.cxx
    R3BLogger.cxx
    R3BModule.cxx
    R3BTcutPar.cxx
    R3BTsplinePar.cxx
    R3BWhiterabbitPropagator.cxx
    ./pars/R3BMSOffsetPar.cxx
    ./pars/R3BMSOffsetContFact.cxx
    ./pars/R3BMSOffsetFinder.cxx
    )

set(HEADERS
    R3BCoarseTimeStitch.h
    R3BDataPropagator.h
    R3BDetector.h
    R3BEventHeader.h
    R3BEventHeaderPropagator.h
    R3BException.h
    R3BFileSource.h
    R3BFileSource2.h
    R3BLogger.h
    R3BModule.h
    R3BShared.h
    R3BTcutPar.h
    R3BTsplinePar.h
    R3BWhiterabbitPropagator.h
    ./pars/R3BMSOffsetPar.h
    ./pars/R3BMSOffsetContFact.h
    ./pars/R3BMSOffsetFinder.h
    )

set(LINKDEF BaseLinkDef.h)

set(DEPENDENCIES
    GeoBase
    ParBase
    Base
    FairTools
    R3BData
    Core
    Geom
    GenVector
    Physics
    Matrix
    MathCore
    Hist
    Graf
    Gpad
    Net
    Imt
    RIO
    RHTTP
    Spectrum
    stdc++fs)

if("${FairRoot_VERSION}" VERSION_GREATER_EQUAL 18.8.0)
    set(DEPENDENCIES ${DEPENDENCIES} Online)
endif()

set(LIBRARY_NAME R3BBase)

generate_library()
