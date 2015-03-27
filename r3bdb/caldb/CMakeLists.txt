# Create a library called "R3BCaloDB" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES 
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
${R3BROOT_SOURCE_DIR}/r3bdb/commondb
${R3BROOT_SOURCE_DIR}/r3bdb/caldb
${R3BROOT_SOURCE_DIR}/r3bdb/r3bdata/calodata
${R3BROOT_SOURCE_DIR}/r3bdb/cal
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
#Put here your sourcefiles
R3BCaloContFact.cxx
R3BCaloMediaPar.cxx
R3BCaloGeometryPar.cxx
R3BGeomAnalyzer.cxx
R3BCaloAddonBoardPar.cxx
R3BCaloAlveolusGeoPar.cxx
R3BCaloAlveolusPar.cxx
R3BCaloAmplifierPar.cxx
R3BCaloAPDPar.cxx
R3BCaloAPDLabPar.cxx
R3BCaloCalPar.cxx
#R3BCaloCalParFinder.cxx
R3BCaloCrystalPar.cxx
R3BCaloCrystalLabPar.cxx
R3BCaloDetectionUnitPar.cxx
R3BCaloDetectionUnitSimPar.cxx
R3BCaloDetectionUnitSimGeoPar.cxx
R3BCaloDetectorPar.cxx
R3BCaloDUCalPar.cxx
R3BCaloFEBEXPar.cxx
R3BCaloPreamplifierPar.cxx
)

CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")
SET(HEADERS ${HEADERS} ${FAIRROOTPATH}/include/FairDbReader.h ${FAIRROOTPATH}/include/FairDbWriter.h)

set(LINKDEF R3BCaloGeoLinkDef.h)
Set(LIBRARY_NAME R3BCaloDB)
Set(DEPENDENCIES Base)

GENERATE_LIBRARY()



