# fairsoft lib path needed to be added for root dictionary detection:

# add library path to LD_LIBRARY_PATH
macro(ADD_TO_LD_LIBRARY_PATH path)
    message(STATUS "Adding ${path}")
    if(EXISTS ${path}/lib)
        list(APPEND LD_LIBRARY_PATH ${path}/lib)
    elseif(EXISTS ${path}/lib64)
        list(APPEND LD_LIBRARY_PATH ${path}/lib64)
    else()
        message(FATAL_ERROR "Cannot resolve the library path from \"${path}\"")
    endif()
endmacro()

add_to_ld_library_path($ENV{SIMPATH})
add_to_ld_library_path($ENV{FAIRROOTPATH})
