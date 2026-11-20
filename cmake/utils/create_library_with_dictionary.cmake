# generate root dictionary from cmake target
function(add_library_with_dictionary)
    set(options)
    set(oneValueArgs LIBNAME LINKDEF)
    set(multiValueArgs
        SRCS
        INCLUDEDIRS
        HEADERS
        PRIVATE_INCLUDEDIRS
        DEPENDENCIES
        PRIVATE_DEPENDENCIES
    )
    cmake_parse_arguments(
        ARGS
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    add_library(${ARGS_LIBNAME} SHARED)
    target_sources(
        ${ARGS_LIBNAME}
        PRIVATE ${ARGS_SRCS}
        PUBLIC FILE_SET HEADERS FILES ${ARGS_HEADERS}
    )

    set_property(
        GLOBAL
        APPEND
        PROPERTY R3BROOT_BUILD_TREE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    )

    foreach(dir IN LISTS ARGS_INCLUDEDIRS)
        if("${dir}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
            continue()
        endif()
        set_property(
            GLOBAL
            APPEND
            PROPERTY
                R3BROOT_BUILD_TREE_INCLUDE_DIRS
                    ${CMAKE_CURRENT_SOURCE_DIR}/${dir}
        )
        target_include_directories(
            ${ARGS_LIBNAME}
            PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${dir}>"
        )
        target_include_directories(
            ${ARGS_LIBNAME}
            PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${dir}>"
        )
    endforeach()

    set(R3BROOT_BUILD_TREE_INCLUDE_DIRS
        ${R3BROOT_BUILD_TREE_INCLUDE_DIRS}
        PARENT_SCOPE
    )

    target_include_directories(
        ${ARGS_LIBNAME}
        PRIVATE ${ARGS_PRIVATE_INCLUDEDIRS}
    )
    target_link_libraries(
        ${ARGS_LIBNAME}
        PUBLIC ${ARGS_DEPENDENCIES}
        PRIVATE ${ARGS_PRIVATE_DEPENDENCIES}
    )
    root_generate_dictionary(
        ${ARGS_LIBNAME}_dict
        ${ARGS_HEADERS}
        MODULE
        ${ARGS_LIBNAME}
        LINKDEF
        ${ARGS_LINKDEF}
        OPTIONS -noIncludePaths
    )
    install(TARGETS ${ARGS_LIBNAME} EXPORT R3BRootLibs FILE_SET HEADERS)
endfunction()

# generate macros using available variables. Please use create_library_with_dictionary instead.
macro(ADD_LIB_WITH_DIC)
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
        ${INCLUDE_DIRS}
        DEPENDENCIES
        ${DEPENDENCIES}
    )
endmacro()
