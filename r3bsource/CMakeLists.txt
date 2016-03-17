# Create a library called "libR3B" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(SYSTEM_INCLUDE_DIRECTORIES 
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bsource
${R3BROOT_SOURCE_DIR}/r3bdata/neulandData
${R3BROOT_SOURCE_DIR}/r3bdata/pspData
${R3BROOT_SOURCE_DIR}/r3bdata/losData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
)


set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF R3BLinkDef.h)

set(DEPENDENCIES
    GeoBase ParBase MbsAPI Base FairTools R3BData Core Geom GenVector Physics Matrix MathCore)


# Include ucesb, if it was found
if(WITH_UCESB)
	set(LINK_DIRECTORIES ${LINK_DIRECTORIES} ${ucesb_LIBRARY_DIR})
	set(SYSTEM_INCLUDE_DIRECTORIES ${SYSTEM_INCLUDE_DIRECTORIES} ${ucesb_INCLUDE_DIR})
	set(DEPENDENCIES ${DEPENDENCIES} ${ucesb_LIBRARY_SHARED})
	set(SRCS
		R3BUcesbSource.cxx
		R3BReader.cxx
		R3BUnpackReader.cxx
		R3BPspReader.cxx
		R3BLosReader.cxx
		R3BTofdReader.cxx
		R3BNeulandTamexReader.cxx
                R3BNeulandTacquilaReader.cxx
	)
message("-- Looking for ucesb... - found ucesb")
endif(WITH_UCESB)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})
link_directories( ${LINK_DIRECTORIES})

set(LIBRARY_NAME R3Bsource)

GENERATE_LIBRARY()

