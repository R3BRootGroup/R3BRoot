# Create a library called "R3BCaloDB" 

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/r3bdb/commondb
${CMAKE_SOURCE_DIR}/r3bdb/caldb
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
R3BCaloContFact.cxx
R3BCaloMediaPar.cxx
R3BCaloAlveolusGeoPar.cxx
R3BCaloAlveolusPar.cxx
R3BCaloCrystalGeoPar.cxx
R3BCaloCrystalPar.cxx
R3BCaloGeometryPar.cxx
R3BGeomAnalyzer.cxx
)

set(HEADERS
R3BCaloContFact.h
R3BCaloMediaPar.h
R3BCaloAlveolusGeoPar.h
R3BCaloAlveolusPar.h
R3BCaloCrystalGeoPar.h
R3BCaloCrystalPar.h
R3BCaloGeometryPar.h
R3BGeomAnalyzer.h
${FAIRROOTPATH}/include/FairDbReader.h
${FAIRROOTPATH}/include/FairDbWriter.h
)


set(LINKDEF R3BCaloGeoLinkDef.h)
Set(LIBRARY_NAME R3BCaloDB)

GENERATE_LIBRARY()



