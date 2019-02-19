#
# Plox to install EPICS base and set EPICS_BASE and EPICS_HOST_ARCH.
#

message(STATUS "Looking for EPICS (base)...")

find_path(EPICS_INCLUDE_DIR
          NAMES cadef.h
          PATHS $ENV{EPICS_BASE}/include
          NO_DEFAULT_PATH
          DOC "Path to EPICS include header files.")

if(EPICS_INCLUDE_DIR)
    set(EPICS_HOST_ARCH $ENV{EPICS_HOST_ARCH})
    if(NOT EPICS_HOST_ARCH)
        set(EPICS_HOST_ARCH linux-x86_64)
    endif(NOT EPICS_HOST_ARCH)

    # Extract ARCH_CLASS from EPICS config/* and look for OS-specific headers.
    file(STRINGS $ENV{EPICS_BASE}/config/CONFIG.Host.${EPICS_HOST_ARCH} EPICS_ARCH_CLASS
         REGEX "ARCH_CLASS[ \t]*=.*")
    string(REGEX
           REPLACE "[^=]*=[ \t]*(.*)"
                   "\\1"
                   EPICS_ARCH_CLASS
                   ${EPICS_ARCH_CLASS})
    find_path(EPICS_OS_INCLUDE_DIR
              NAMES osdEvent.h
              PATHS $ENV{EPICS_BASE}/include/os/${EPICS_ARCH_CLASS}
              NO_DEFAULT_PATH
              DOC "Path to EPICS OS include header files.")

    find_path(EPICS_LIBRARY_DIR
              NAMES "libca.so"
              PATHS $ENV{EPICS_BASE}/lib/${EPICS_HOST_ARCH}
              NO_DEFAULT_PATH
              DOC "Path to EPICS library files.")
endif(EPICS_INCLUDE_DIR)

if(EPICS_INCLUDE_DIR AND EPICS_OS_INCLUDE_DIR AND EPICS_LIBRARY_DIR)
    set(EPICS_FOUND true)
else(EPICS_INCLUDE_DIR AND EPICS_OS_INCLUDE_DIR AND EPICS_LIBRARY_DIR)
    set(EPICS_FOUND false)
endif(EPICS_INCLUDE_DIR AND EPICS_OS_INCLUDE_DIR AND EPICS_LIBRARY_DIR)

message("    - EPICS_BASE = $ENV{EPICS_BASE}")
message("    - EPICS_INCLUDE_DIR   = ${EPICS_INCLUDE_DIR}")
message("    - EPICS_OSINCLUDE_DIR = ${EPICS_OS_INCLUDE_DIR}")
message("    - EPICS_LIBRARY_DIR   = ${EPICS_LIBRARY_DIR}")

if(EPICS_FOUND)
    add_definitions(-DWITH_EPICS)
    message("-- Looking for EPICS... - FOUND")
else(EPICS_FOUND)
    message("-- Looking for EPICS... - MISSING")
endif(EPICS_FOUND)
