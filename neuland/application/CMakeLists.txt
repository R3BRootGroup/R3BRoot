add_library(R3BNeulandApp SHARED R3BNeulandAnalysisApp.cxx R3BNeulandApp.cxx
                                 R3BNeulandAppOptionJson.cxx R3BNeulandSimApp.cxx)

target_include_directories(R3BNeulandApp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    R3BNeulandApp
    PRIVATE magic_enum::magic_enum
    PUBLIC R3BNeulandDigitizing
           nlohmann_json::nlohmann_json
           R3BNeulandReconstruction
           R3BNeulandSimulation
           R3BNeulandClustering
           ${Geant4_LIBRARIES}
           ${Geant4VMC_LIBRARIES}
           R3BGen
           CLI11::CLI11)
