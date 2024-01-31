add_library(R3BMille SHARED Mille.cpp)
target_include_directories(R3BMille PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(R3BMille PRIVATE fmt::fmt range-v3::range-v3)
