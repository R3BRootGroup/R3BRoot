# Create a library called "libPassive" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/field
${R3BROOT_SOURCE_DIR}/passive
${R3BROOT_SOURCE_DIR}/tracking )

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
R3BCave.cxx
R3BGeoCave.cxx
R3BGeoGDML.cxx
R3BGeoMagnet.cxx
R3BGeoPassivePar.cxx
R3BGeoPipe.cxx
R3BGeoVacVesselCool.cxx
R3BGeoTarget.cxx
R3BAladinMagnet.cxx
R3BGladMagnet.cxx
R3BPassiveContFact.cxx
R3BPipe.cxx
R3BVacVesselCool.cxx
R3BTarget.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF PassiveLinkDef.h)
Set(LIBRARY_NAME R3BPassive)
Set(DEPENDENCIES
    R3Bbase Field R3BTracking)

GENERATE_LIBRARY()
