# ##################################################################################################
# Copyright (C) 2023 GSI Helmholtzzentrum für Schwerionenforschung GmbH    # Copyright (C) 2023-2025
# Members of R3B Collaboration                     # # This software is distributed under the terms
# of the            # GNU General Public Licence (GPL) version 3,                # copied verbatim
# in the file "LICENSE".                  # # In applying this license GSI does not waive the
# privileges and immunities  # granted to it by virtue of its status as an Intergovernmental
# Organization # or submit itself to any jurisdiction.                                      #
# ##################################################################################################

if(BUILD_GEOMETRY)
    if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/neuland_12dp.geo.root")
        message(
            STATUS
            "Starting to generate the standard root geometry v2021.3 with 12DP for NeuLAND"
        )
        execute_process(
            COMMAND
                root -l -q -x
                "${R3BROOT_SOURCE_DIR}/neuland/geobase/create_neuland_geo.C(24)"
        )
        message(
            STATUS
            "${BYellow}NeuLAND standard root geometry v2021.3 with 12DP has been created${CR} "
        )
    else()
        message(
            STATUS
            "${BYellow}NeuLAND standard root geometry v2021.3 with 12DP already created${CR} "
        )
    endif()

    if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/neuland_13dp.geo.root")
        message(
            STATUS
            "Starting to generate the standard root geometry v2022.5 with 13DP for NeuLAND"
        )
        execute_process(
            COMMAND
                root -l -q -x
                "${R3BROOT_SOURCE_DIR}/neuland/geobase/create_neuland_geo.C(26)"
        )
        message(
            STATUS
            "${BYellow}NeuLAND standard root geometry v2022.5 with 13DP has been created${CR} "
        )
    else()
        message(
            STATUS
            "${BYellow}NeuLAND standard root geometry v2022.5 with 13DP already created${CR} "
        )
    endif()

    if(NOT EXISTS "${R3BROOT_SOURCE_DIR}/geometry/neuland_30dp.geo.root")
        message(
            STATUS
            "Starting to generate the standard root geometry for NeuLAND with 30DP"
        )
        execute_process(
            COMMAND
                root -l -q -x
                "${R3BROOT_SOURCE_DIR}/neuland/geobase/create_neuland_geo.C(60)"
        )
        message(
            STATUS
            "${BYellow}NeuLAND standard root geometry with 30DP has been created${CR} "
        )
    else()
        message(
            STATUS
            "${BYellow}NeuLAND standard root geometry with 30DP already created${CR} "
        )
    endif()

    generate_root_test_script(${R3BROOT_SOURCE_DIR}/neuland/geobase/create_neuland_geo.C)
    add_test(
        NAME NeulandRootGeometry
        COMMAND ${R3BROOT_BINARY_DIR}/neuland/geobase/create_neuland_geo.sh
    )
    set_tests_properties(NeulandRootGeometry PROPERTIES TIMEOUT "2000")
    set_tests_properties(
        NeulandRootGeometry
        PROPERTIES PASS_REGULAR_EXPRESSION "Macro finished successfully."
    )
endif()

add_library(R3BNeulandGeoBase SHARED R3BNeulandGeoCreator.cxx)
target_include_directories(R3BNeulandGeoBase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    R3BNeulandGeoBase
    PUBLIC R3BNeulandShared FairRoot::GeoBase
)
