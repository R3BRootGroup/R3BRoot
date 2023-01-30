  message(STATUS "Detected c3w configuration. Setting default target to c3w tar file.")
  message(STATUS "Run make all to build locally only.")
  execute_process(COMMAND ln -vs ${CMAKE_SOURCE_DIR}/util/c3w/c3w.make ${CMAKE_CURRENT_BINARY_DIR}/../../GNUmakefile)
  execute_process(COMMAND ln -vs ${CMAKE_SOURCE_DIR}/util/c3w/local_env.sh ${CMAKE_CURRENT_BINARY_DIR}/../../local_c3w.sh)
