add_subdirectory(templates)

add_executable(neulandSim neulandSim.cxx)
target_link_libraries(neulandSim PUBLIC R3B::Neuland ExTestDetector)

# set(EXE_NAME neulandSim)
# set(DEPENDENCIES
#     R3BNeulandShared
#     R3BNeulandSimulation
#     R3BPassive
#     R3BData
#     R3BBase
#     Gen
#     ${Geant4_LIBRARIES}
#     ${Geant4VMC_LIBRARIES}
#     ExTestDetector
#     Boost::program_options)

# set(INCLUDE_DIRECTORIES
#     ${INCLUDE_DIRECTORIES} ${R3BROOT_SOURCE_DIR}/passive ${R3BROOT_SOURCE_DIR}/neuland/executables
#     ${R3BROOT_SOURCE_DIR}/neuland/simulation ${Geant4_INCLUDE_DIRS} ${Geant4VMC_INCLUDE_DIRS})

add_executable(neulandSim neulandSim.cxx)
target_link_libraries(neulandSim PUBLIC R3BNeulandSimulation R3BNeulandShared ${Geant4_LIBRARIES}
                                        ${Geant4VMC_LIBRARIES} ExTestDetector R3BTracking)

add_executable(neulandAna neulandAna.cxx)
target_link_libraries(neulandAna PUBLIC R3BData R3BNeulandShared R3BNeulandDigitizing)

# set(EXE_NAME neulandAna)
# set(DEPENDENCIES
#     R3BNeulandShared
#     R3BNeulandDigitizing
#     R3BPassive
#     R3BData
#     R3BBase
#     Gen
#     Boost::program_options)

# set(INCLUDE_DIRECTORIES
#     ${INCLUDE_DIRECTORIES} ${R3BROOT_SOURCE_DIR}/passive ${R3BROOT_SOURCE_DIR}/neuland/executables
#     ${R3BROOT_SOURCE_DIR}/neuland/digitizing ${R3BROOT_SOURCE_DIR}/neuland/calibration)

# include_directories(${INCLUDE_DIRECTORIES})
# set(SRCS neulandAna.cxx)

# generate_executable()

# add_subdirectory(templates)
