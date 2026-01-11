##############################################################################
#   Copyright (C) 2020 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2020-2026 Members of R3B Collaboration                     #
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
R3BAtima.cxx
R3BAtimaCache.cxx
)

set(HEADERS
R3BAtima.h
R3BAtimaCache.h
)

add_library_with_dictionary(
    LIBNAME
    R3BAtima
    LINKDEF
    AtimaLinkDef.h
    HEADERS
    ${HEADERS}
    SRCS
    ${SRCS}
    INCLUDEDIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDENCIES
    Atima::Atima R3BBase)

add_subdirectory(test)
