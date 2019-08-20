# Note the Fortran project mention in the main CMakeLists.txt.
include(CheckFortran)
add_library(R3BTraRene SHARED tracker_routines.f95)
