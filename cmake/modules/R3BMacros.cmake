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

include(FairRootSummary)
include(FairRootTargetRootDictionary)
include(FairRootUtils)

macro(r3broot_summary)
  fairroot_summary_spacer()
  fairroot_summary_global_cxx_flags()
  fairroot_summary_spacer()
  fairroot_summary_cxx_standard()
  fairroot_summary_spacer()
  fairroot_summary_package_dependencies()
  fairroot_summary_spacer()
  fairroot_summary_build_types()
  fairroot_summary_spacer()
  fairroot_summary_install_prefix()
  fairroot_summary_spacer()
endmacro()

macro(r3broot_add_library target)
  cmake_parse_arguments(PARSE_ARGV 1 ARGS
                        "NO_GUESS_HEADERS"
                        "LINKDEF"
                        "HEADERS;SOURCES;LINKS")
  if(NOT ARGS_NO_GUESS_HEADERS)
    fairroot_change_extensions_if_exists(.cxx .h
      FILES "${ARGS_SOURCES}" OUTVAR headers)
    set(headers ${headers} ${ARGS_HEADERS})
    list(REMOVE_DUPLICATES ${headers})
  else()
    set(headers ${ARGS_HEADERS})
  endif()

  add_library(${target} ${ARGS_SOURCES} ${headers} ${ARGS_HEADERS})
  add_library(${PROJECT_NAME_LOWER}::${target} ALIAS ${target})
  set_target_properties(${target} PROPERTIES ${PROJECT_LIBRARY_PROPERTIES})

  target_include_directories(${target} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${PROJECT_INSTALL_INCDIR}>)

  if(ARGS_LINKS)
    target_link_libraries(${target} ${ARGS_LINKS})
  endif()

  if(ARGS_LINKDEF)
    if(headers)
      fairroot_target_root_dictionary(${target}
        HEADERS ${headers}
        LINKDEF R3BLinkDef.h)
    else()
      fairroot_target_root_dictionary(${target}
        LINKDEF R3BLinkDef.h)
    endif()
  endif()

  install(TARGETS ${target}
    EXPORT ${PROJECT_EXPORT_SET}
    LIBRARY DESTINATION ${PROJECT_INSTALL_LIBDIR})
  if(headers)
    install(FILES ${headers}
      DESTINATION ${PROJECT_INSTALL_INCDIR})
  endif()
endmacro()
