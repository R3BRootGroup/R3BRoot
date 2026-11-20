macro(GENERATE_ROOT_TEST_SCRIPT SCRIPT_FULL_NAME)
    get_filename_component(path_name ${SCRIPT_FULL_NAME} PATH)
    get_filename_component(file_extension ${SCRIPT_FULL_NAME} EXT)
    get_filename_component(file_name ${SCRIPT_FULL_NAME} NAME_WE)
    set(shell_script_name "${file_name}.sh")

    #MESSAGE("PATH: ${path_name}")
    #MESSAGE("Ext: ${file_extension}")
    #MESSAGE("Name: ${file_name}")
    #MESSAGE("Shell Name: ${shell_script_name}")

    string(
        REPLACE ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR}
        new_path
        ${path_name}
    )

    #MESSAGE("New PATH: ${new_path}")

    file(MAKE_DIRECTORY ${new_path}/data)

    convert_list_to_string(${LD_LIBRARY_PATH})
    set(MY_LD_LIBRARY_PATH ${output})

    convert_list_to_string(${ROOT_INCLUDE_PATH})
    set(MY_ROOT_INCLUDE_PATH ${output})

    set(my_script_name ${SCRIPT_FULL_NAME})

    write_geant4data_variables_sh()
    configure_file(
        ${PROJECT_SOURCE_DIR}/cmake/scripts/rootplus_macro.sh.in
        ${new_path}/${shell_script_name}
    )
    execute_process(
        COMMAND /bin/chmod u+x ${new_path}/${shell_script_name}
        OUTPUT_QUIET
    )
endmacro(GENERATE_ROOT_TEST_SCRIPT)
