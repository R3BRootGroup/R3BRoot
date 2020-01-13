# FindAtima.cmake
#
# Finds the Atima library
#
# This will define the following variables
#
# Atima_FOUND Atima_LIBRARY
#
# and the following imported targets
#
# Atima::Atima
#

find_library(Atima_LIBRARY
             NAMES Atima
             HINTS $ENV{ATIMAPATH})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Atima
                                  FOUND_VAR
                                  Atima_FOUND
                                  REQUIRED_VARS
                                  Atima_LIBRARY)

if(Atima_FOUND AND NOT TARGET Atima::Atima)
    message("-- Found Atima: ${Atima_LIBRARY}")
    add_library(Atima::Atima UNKNOWN IMPORTED)
    set_target_properties(Atima::Atima PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${Atima_LIBRARY}"
    )
endif()
