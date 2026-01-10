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

################################################################################
# FindAtima.cmake - Find module for CaTima / Atima using $ENV{ATIMAPATH}
################################################################################

message(STATUS "Looking for ATIMA...")

set(Atima_INCLUDE_DIR Atima_INCLUDE_DIR-NOTFOUND)
set(Atima_LIBRARY Atima_LIBRARY-NOTFOUND)
set(Atima_VERSION "")

if(NOT DEFINED ENV{ATIMAPATH})
    message("    - Environment variable ATIMAPATH is not set.")
else()
    set(CATIMA_ROOT $ENV{ATIMAPATH})

    find_path(Atima_INCLUDE_DIR
        NAMES catima/catima.h
        HINTS ${CATIMA_ROOT}/include
        PATH_SUFFIXES catima
        DOC "Path to ATIMA include directory"
    )

    find_library(Atima_LIBRARY_SHARED
        NAMES catima libcatima
        HINTS ${CATIMA_ROOT}/lib
        DOC "Path to ATIMA shared library"
    )

    if(Atima_LIBRARY_SHARED)
        set(Atima_LIBRARY ${Atima_LIBRARY_SHARED})
    endif()

    if(EXISTS "${CATIMA_ROOT}/share/catima/catimaConfigVersion.cmake")
        include("${CATIMA_ROOT}/share/catima/catimaConfigVersion.cmake")
        if(DEFINED PACKAGE_VERSION)
            set(Atima_VERSION "${PACKAGE_VERSION}")
        endif()
    endif()

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Atima
        REQUIRED_VARS Atima_LIBRARY Atima_INCLUDE_DIR
        VERSION_VAR Atima_VERSION
    )

    if(ATIMA_FOUND AND NOT TARGET Atima::Atima)
        add_library(Atima::Atima UNKNOWN IMPORTED GLOBAL)
        set_target_properties(Atima::Atima PROPERTIES
           IMPORTED_LOCATION "${Atima_LIBRARY}"
           INTERFACE_INCLUDE_DIRECTORIES "${Atima_INCLUDE_DIR}"
           INTERFACE_LINK_LIBRARIES "${Atima_LIBRARY}"
        )
    endif()
endif()

message("    - ${Cyan}ATIMAPATH${CR} = ${BGreen}$ENV{ATIMAPATH}${CR}")
message("    - ${Cyan}ATIMA_INCLUDE_DIR${CR} = ${BGreen}${Atima_INCLUDE_DIR}${CR}")
message("    - ${Cyan}ATIMA_LIBRARY${CR} = ${BGreen}${Atima_LIBRARY}${CR}")

if(ATIMA_FOUND)
  message(STATUS "${BGreen}ATIMA was FOUND${CR} (version ${Atima_VERSION})")
else()
  message(STATUS "${BYellow}Could not find package ATIMA${CR}")
endif()
