if(CONAN_ENABLED)
    find_package(MPI COMPONENTS C CXX)

    add_executable(neuland neuland_cli.cxx)
    target_link_libraries(neuland PRIVATE R3BNeulandApp CLI11::CLI11)
    target_compile_definitions(neuland PRIVATE JSON_USE_IMPLICIT_CONVERSIONS=0)

    if(MPI_FOUND)
        target_link_libraries(neuland PRIVATE MPI::MPI_CXX)
        target_compile_definitions(neuland PRIVATE HAS_MPI)
    endif()

    add_executable(neuland_online_monitor neuland_online_monitor.cxx)
    target_link_libraries(
        neuland_online_monitor
        PRIVATE R3BNeulandOnline R3BLos R3BSource -pthread
    )

    install(TARGETS neuland neuland_online_monitor)
endif()
