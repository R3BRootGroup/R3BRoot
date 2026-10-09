# generate root dictionary from cmake target
function(add_library_with_dictionary)
    set(options)
    set(oneValueArgs LIBNAME LINKDEF)
    set(multiValueArgs SRCS INCLUDEDIRS HEADERS PRIVATE_INCLUDEDIRS DEPENDENCIES
                       PRIVATE_DEPENDENCIES)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 1. Create the shared library target
    add_library(${ARGS_LIBNAME} SHARED ${ARGS_SRCS})

	# Automatically copy PCM and rootmap files to build/lib alongside .so files
	add_custom_command(
		TARGET ${ARGS_LIBNAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
				${CMAKE_CURRENT_BINARY_DIR}/lib${ARGS_LIBNAME}_rdict.pcm
				${CMAKE_BINARY_DIR}/lib/lib${ARGS_LIBNAME}_rdict.pcm
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
				${CMAKE_CURRENT_BINARY_DIR}/lib${ARGS_LIBNAME}.rootmap
				${CMAKE_BINARY_DIR}/lib/lib${ARGS_LIBNAME}.rootmap
		COMMENT "Staging ${ARGS_LIBNAME} PCM and ROOTMAP to build/lib"
	)
	
    # 2. Set include directories for the target
    target_include_directories(
        ${ARGS_LIBNAME}
        PUBLIC $<BUILD_INTERFACE:${ARGS_INCLUDEDIRS}>
               $<INSTALL_INTERFACE:include>
        PRIVATE ${ARGS_PRIVATE_INCLUDEDIRS})

    # 3. Link dependencies
    target_link_libraries(
        ${ARGS_LIBNAME}
        PUBLIC ${ARGS_DEPENDENCIES}
        PRIVATE ${ARGS_PRIVATE_DEPENDENCIES})

    # 4. Set directory include property so rootcling finds headers
    set_property(DIRECTORY PROPERTY INCLUDE_DIRECTORIES ${ARGS_INCLUDEDIRS})

    # 5. Generate ROOT dictionary (.pcm and G__*.cxx)
    root_generate_dictionary(
        ${ARGS_LIBNAME}_dict 
        ${ARGS_HEADERS} 
        MODULE ${ARGS_LIBNAME} 
        LINKDEF ${ARGS_LINKDEF}
    )

    # 6. Install headers and dictionary PCM files automatically
    install(TARGETS ${ARGS_LIBNAME} DESTINATION lib)
    install(FILES ${ARGS_HEADERS} DESTINATION include)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${ARGS_LIBNAME}_rdict.pcm DESTINATION lib OPTIONAL)
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
        ${DEPENDENCIES})
endmacro()

