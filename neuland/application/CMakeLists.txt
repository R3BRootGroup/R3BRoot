add_library(R3BNeulandApp STATIC)

target_sources(
    R3BNeulandApp
    PRIVATE
        R3BNeulandAnalysisApp.cxx
        R3BNeulandApp.cxx
        R3BNeulandBaseParCreator.cxx
        R3BNeulandSimApp.cxx
    PUBLIC
        FILE_SET HEADERS
            FILES
                R3BNeulandAnalysisApp.h
                R3BNeulandApp.h
                R3BNeulandBaseParCreator.h
                R3BNeulandCLIAbstract.h
                R3BNeulandSimApp.h
)

target_compile_definitions(
    R3BNeulandApp
    PUBLIC BOOST_PROCESS_USE_STD_FS=1
    PRIVATE BOOST_PROCESS_V2_SEPARATE_COMPILATION=1
)

target_link_libraries(
    R3BNeulandApp
    PRIVATE magic_enum::magic_enum
    PUBLIC
        R3BNeulandDigitizing
        R3BNeulandReconstruction
        R3BLos
        R3BNeulandSimulation
        R3BNeulandClustering
        ${Geant4_LIBRARIES}
        ${Geant4VMC_LIBRARIES}
        R3BGen
        CLI11::CLI11
        glaze::glaze
        re2::re2
)
