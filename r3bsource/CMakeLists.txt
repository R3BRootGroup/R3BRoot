# Create a library called "libR3B" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(SYSTEM_INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
${ucesb_INCLUDE_DIR}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bsource
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/r3bdata/califaData
${R3BROOT_SOURCE_DIR}/r3bdata/neulandData
${R3BROOT_SOURCE_DIR}/r3bdata/pspData
${R3BROOT_SOURCE_DIR}/r3bdata/fibData
${R3BROOT_SOURCE_DIR}/r3bdata/losData
${R3BROOT_SOURCE_DIR}/r3bdata/tofData
${R3BROOT_SOURCE_DIR}/r3bdata/strawtubesData
${R3BROOT_SOURCE_DIR}/psp/par
${R3BROOT_SOURCE_DIR}/r3bdata/startrackData
)

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${ucesb_LIBRARY_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})
link_directories( ${LINK_DIRECTORIES})

set(SRCS
R3BUcesbSource.cxx
R3BReader.cxx
R3BUnpackReader.cxx
R3BWhiterabbitReader.cxx
R3BPspxReader.cxx
R3BBunchedFiberReader.cxx
R3BFi0Reader.cxx
#R3BFi1aReader.cxx
#R3BFi1bReader.cxx
#R3BFi2aReader.cxx
#R3BFi2bReader.cxx
#R3BFi3aReader.cxx
#R3BFi3bReader.cxx
R3BFi4Reader.cxx
R3BFi5Reader.cxx
R3BFi6Reader.cxx
R3BFi7Reader.cxx
#R3BFi8Reader.cxx
#R3BFi9Reader.cxx
#R3BFi10Reader.cxx
#R3BFi11Reader.cxx
R3BLosReader.cxx
R3BTofdReader.cxx
R3BCalifaFebexReader.cxx
R3BPtofReader.cxx
R3BNeulandTamexReader.cxx
R3BNeulandTacquilaReader.cxx
R3BStrawtubesReader.cxx
R3BStartrackReader.cxx
)

Set(STRUCT_HEADERS
ext_h101_unpack.h
ext_h101_whiterabbit.h
ext_h101_raw_nnp.h
ext_h101_fibzero.h
#ext_h101_fibonea.h
#ext_h101_fiboneb.h
#ext_h101_fibtwoa.h
#ext_h101_fibtwob.h
#ext_h101_fibthreea.h
#ext_h101_fibthreeb.h
ext_h101_fibfour.h
ext_h101_fibfive.h
ext_h101_fibsix.h
ext_h101_fibseven.h
#ext_h101_fibeight.h
#ext_h101_fibnine.h
#ext_h101_fibten.h
#ext_h101_fibeleven.h
ext_h101_los.h
ext_h101_los_tamex.h
ext_h101_tofd.h
ext_h101_ptof.h
ext_h101_psp.h
ext_h101_raw_califa_febex.h
ext_h101_raw_nnp_tamex.h
ext_h101_strawtubes.h
ext_h101_star.h
ext_h101_fibfour.h
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")
Set(HEADERS ${STRUCT_HEADERS} ${HEADERS})

set(LINKDEF_FILE R3BLinkDef.h)
set(DEPENDENCIES
    GeoBase ParBase MbsAPI Base FairTools
    R3Bbase R3BData Core Geom GenVector
    Physics Matrix MathCore R3BPsp
    ${ucesb_LIBRARY_SHARED})
set(LIBRARY_NAME R3Bsource)

If(ROOT_FOUND_VERSION LESS 59999)
  Set(DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/G__${LIBRARY_NAME}Dict.cxx)
  ROOT_GENERATE_DICTIONARY_OLD_EXTRA("${HEADERS}" "${LINKDEF_FILE}" "${DICTIONARY}" "${INCLUDE_DIRECTORIES}${SYSTEM_INCLUDE_DIRECTORIES}")
  Set(SRCS ${SRCS} ${DICTIONARY})
  CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")
  Set(HEADERS ${STRUCT_HEADERS} ${HEADERS})
  Set(LINKDEF ${LINKDEF_FILE})
  ROOT_GENERATE_ROOTMAP()
  Set(LINKDEF )
Else(ROOT_FOUND_VERSION LESS 59999)
  Set(LINKDEF ${LINKDEF_FILE})
EndIf(ROOT_FOUND_VERSION LESS 59999)

GENERATE_LIBRARY()
