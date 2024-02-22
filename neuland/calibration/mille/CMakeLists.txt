find_package(Eigen3)

add_library(R3BMille SHARED Mille.cxx SteerWriter.cxx)
target_include_directories(R3BMille PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(R3BMille PRIVATE fmt::fmt range-v3::range-v3 Eigen3::Eigen)
