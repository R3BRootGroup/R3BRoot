set(EXE_NAME ex_digi)
set(DEPENDENCIES
    # cmake-format: sortable
    Boost::program_options
    Gen
    R3B::Neuland
    R3BBase
    R3BData
    R3BNeulandClustering
    R3BNeulandDigitizing
    R3BNeulandShared
    R3BPassive)

set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES}
    ${R3BROOT_SOURCE_DIR}/passive
    ${R3BROOT_SOURCE_DIR}/neuland/executables
    ${R3BROOT_SOURCE_DIR}/neuland/digitizing
    ${R3BROOT_SOURCE_DIR}/neuland/calibration)

include_directories(${INCLUDE_DIRECTORIES})
set(SRCS ex_digi.cxx)

generate_executable()
# ============================================================================

# set(EXE_NAME ex_reader) set(DEPENDENCIES # cmake-format: sortable Gen R3B::Neuland R3BBase R3BData
# R3BNeulandCalibration R3BNeulandShared R3BPassive R3Bsource R3BTCal UCESB)

# set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} ${R3BROOT_SOURCE_DIR}/passive
# ${R3BROOT_SOURCE_DIR}/tcal)

# include_directories(${INCLUDE_DIRECTORIES} ${Boost_INCLUDE_DIRS}) set(SRCS ex_reader.cxx)

# generate_executable()
# ====================================================================

add_executable(neuland_lmd_to_map neuland_lmd_to_map.cxx)
target_link_libraries(neuland_lmd_to_map PUBLIC R3B::Neuland R3Bsource -pthread)

add_executable(neuland_map_to_cal neuland_map_to_cal.cxx)
target_link_libraries(neuland_map_to_cal PUBLIC R3B::Neuland)

add_executable(neuland_cal_to_hit neuland_cal_to_hit.cxx)
target_link_libraries(neuland_cal_to_hit PUBLIC R3B::Neuland)

add_executable(neuland_online_monitor neuland_online_monitor.cxx)
target_link_libraries(
    neuland_online_monitor
    PUBLIC R3B::Neuland
           R3Bsource
           R3BLos
           -pthread)

add_executable(neuland_par_merger neuland_par_merger.cxx)
target_link_libraries(neuland_par_merger PUBLIC R3B::Neuland)
# ====================================================================

set(EXE_NAME ex_test)
set(DEPENDENCIES
    R3Bsource
    R3BNeulandCalibrationLegacy
    R3BNeulandShared
    R3BPassive
    R3BData
    R3BBase
    R3BTCal
    Gen
    UCESB
    range-v3::range-v3)

set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} ${R3BROOT_SOURCE_DIR}/passive
                        ${R3BROOT_SOURCE_DIR}/tcal)

include_directories(${INCLUDE_DIRECTORIES} ${Boost_INCLUDE_DIRS})
set(SRCS ex_test.cxx)
generate_executable()
# ====================================================================
