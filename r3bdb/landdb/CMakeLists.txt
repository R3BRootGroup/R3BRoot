# Create a library called "R3BLandDB" 

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/r3bdb/commondb
${CMAKE_SOURCE_DIR}/r3bdb/landdb
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
R3BDBLandContFact.cxx
R3BLandCalPar.cxx
R3BLandTCalPar.cxx
)

set(HEADERS
R3BDBLandContFact.h
R3BLandCalPar.h
R3BLandTCalPar.h
${FAIRROOTPATH}/include/FairDbReader.h
${FAIRROOTPATH}/include/FairDbWriter.h
)


set(LINKDEF R3BLandLinkDef.h)
Set(LIBRARY_NAME R3BLandDB)
Set(DEPENDENCIES Base)


GENERATE_LIBRARY()



