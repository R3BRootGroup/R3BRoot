function(check_test_model MODEL)
    if(NOT (${MODEL} MATCHES "^(Nightly|Continuous|Experimental)$"))
        message(
            FATAL_ERROR
                "Ctest model \"${MODEL}\" is neither \"Nightly\", \"Continuous\" nor \"Experimental\"!"
        )
    endif()
endfunction()

macro(set_input_options)
    if(DEFINED BUILD_J)
        set(number_of_processors ${BUILD_J})
    else()
        message("BUILD_J is not specified. Default to 1")
        set(number_of_processors 1)
    endif()

    if(DEFINED TEST_NAME)
        set(CTEST_BUILD_NAME "${TEST_NAME}")
    else()
        message("TEST_NAME is not specified. Default to \"R3BRoot CI/CD\"")
        set(CTEST_BUILD_NAME "R3BRoot CI/CD")
    endif()

    if(DEFINED SITE_NAME)
        set(CTEST_SITE ${SITE_NAME})
    else()
        site_name(CTEST_SITE)
    endif()

    if(DEFINED TEST_MODEL)
        set(ctest_model ${TEST_MODEL})
    else()
        message("TEST_MODEL is not specified. Default to \"Experimental\"")
        set(ctest_model "Experimental")
    endif()

    if(DEFINED BUILD_TYPE)
        set(configure_options "${configure_options};-DCMAKE_BUILD_TYPE=${BUILD_TYPE}")
    endif()
endmacro()

macro(enable_launcher launch)
    if(${launch})
        message(NOTICE "CTEST_USE_LAUNCHERS is enabled.")
        set(CTEST_USE_LAUNCHERS 1)
        set(configure_options "${configure_options};-DCTEST_USE_LAUNCHERS=${CTEST_USE_LAUNCHERS}")
    endif()
endmacro()

macro(cdash_start)
    # clear the build folder for nightly and continuous mode
    if("${ctest_model}" MATCHES "Nightly" OR "${ctest_model}" MATCHES "Continuous")

        find_program(GCOV_COMMAND gcov)
        if(GCOV_COMMAND)
            message("Found GCOV: ${GCOV_COMMAND}")
            set(CTEST_COVERAGE_COMMAND ${GCOV_COMMAND})
        endif(GCOV_COMMAND)
        # ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
    endif()

    execute_process(COMMAND "make" "clean" WORKING_DIRECTORY ${CTEST_BINARY_DIRECTORY})
    ctest_read_custom_files("${CTEST_SOURCE_DIRECTORY}")

    # ctest_start(${ctest_model} GROUP ${ctest_model})
    ctest_start(${ctest_model})
endmacro()

macro(cdash_configure)
    # set(ENV{WERROR} TRUE)
    set(configure_options "${configure_options};-DDISABLE_COLOR=ON")
    if(DEFINED ${EXTRA_FLAGS})
        set(configure_options "${configure_options};${EXTRA_FLAGS}")
    endif()

    # set(CTEST_CONFIGURE_COMMAND
    #     "${CMAKE_COMMAND} -C ${CTEST_SOURCE_DIRECTORY}/cmake/CI_CD/configure_options.cmake -B ${CTEST_BINARY_DIRECTORY} -S ${CTEST_SOURCE_DIRECTORY}")
    ctest_configure(OPTIONS "${configure_options}" RETURN_VALUE _ctest_configure_ret_val)
endmacro()

macro(cdash_build)
    ctest_build(
        BUILD "${CTEST_BINARY_DIRECTORY}"
        PARALLEL_LEVEL ${number_of_processors}
        RETURN_VALUE _ctest_build_ret_val)

endmacro()

macro(cdash_test)
    # execute_process(COMMAND sh "${CTEST_SOURCE_DIRECTORY}/build/config.sh")
    # execute_process(COMMAND sh "${CTEST_SOURCE_DIRECTORY}/util/generate_geo_test.sh")
    ctest_test(
        BUILD "${CTEST_BINARY_DIRECTORY}"
        PARALLEL_LEVEL ${number_of_processors}
        RETURN_VALUE _ctest_test_ret_val)

    if(GCOV_COMMAND)
        ctest_coverage(BUILD "${CTEST_BINARY_DIRECTORY}")
    endif()
endmacro()

macro(print_error)
    # if(_ctest_configure_ret_val)
    #     message(WARNING "cmake failed. Will paste the output."
    #                     "\n---------------------------------------------------\n\n")
    #     execute_process(COMMAND "find" "${CTEST_BINARY_DIRECTORY}" "-iname" "LastConfigure*.log"
    #                             "-exec" /usr/bin/cat "{}" ";")

    #     message(FATAL_ERROR "\n---------------------------------------------------\n"
    #                         "Ctest Failed: Configure failed. (cmake output is above.)")
    # endif()

    # if(_ctest_build_ret_val)
    #     message(WARNING "Build failed. Will paste the error message from the log now."
    #                     "\n---------------------------------------------------\n\n")
    #     execute_process(COMMAND "find" "${CTEST_BINARY_DIRECTORY}" "-iname" "LastBuild_*.log"
    #                             "-exec" ${CTEST_SOURCE_DIRECTORY}/util/print_errors.py "{}" ";")

    #     message(FATAL_ERROR "\n---------------------------------------------------\n"
    #                         "Ctest Failed: Build failed. (make output is above.)")
    # endif()

    if(_ctest_test_ret_val)
        message(ERROR "Some tests failed, printing output of failed tests:")
        execute_process(WORKING_DIRECTORY "${CTEST_BINARY_DIRECTORY}" COMMAND ctest --rerun-failed
                                                                              --output-on-failure)
        message(FATAL_ERROR "End of failed tests output.")
    endif()
endmacro()
