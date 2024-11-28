if(CONAN_ENABLED)
    find_package(MPI COMPONENTS C CXX)

    add_executable(neuland neuland_cli.cxx)
    target_link_libraries(neuland PRIVATE R3BNeulandApp CLI11::CLI11)

    if(MPI_FOUND)
        target_link_libraries(neuland PRIVATE MPI::MPI_CXX)
        target_compile_definitions(neuland PRIVATE HAS_MPI)
    endif()

    add_executable(neuland_lmd_to_map neuland_lmd_to_map.cxx)
    target_link_libraries(neuland_lmd_to_map PRIVATE R3BNeulandCalibration R3BSource -pthread)

    add_executable(neuland_map_to_cal neuland_map_to_cal.cxx)
    target_link_libraries(neuland_map_to_cal PRIVATE R3BNeulandCalibration R3BSource)

    add_executable(neuland_cal_to_hit neuland_cal_to_hit.cxx)
    target_link_libraries(neuland_cal_to_hit PRIVATE R3BNeulandCalibration R3BSource)

    add_executable(neuland_online_monitor neuland_online_monitor.cxx)
    target_link_libraries(neuland_online_monitor PRIVATE R3BNeulandOnline R3BLos R3BSource -pthread)

    add_executable(neuland_par_merger neuland_par_merger.cxx)
    target_link_libraries(neuland_par_merger PRIVATE R3BNeulandShared)

    add_executable(neuland_create_base_par neuland_create_base_par.cxx)
    target_link_libraries(neuland_create_base_par PRIVATE R3BNeulandCalibration R3BNeulandShared)
endif()
