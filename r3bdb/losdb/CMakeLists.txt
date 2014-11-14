# Create a library called "R3BLandDB" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/r3bdb/commondb
${CMAKE_SOURCE_DIR}/r3bdb/losdb
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
R3BDBLosContFact.cxx
R3BLosCalPar.cxx
R3BLosTCalPar.cxx
)

set(HEADERS
R3BDBLosContFact.h
R3BLosCalPar.h
R3BLosTCalPar.h
${FAIRROOTPATH}/include/FairDbReader.h
${FAIRROOTPATH}/include/FairDbWriter.h
)


set(LINKDEF R3BLosLinkDef.h)
Set(LIBRARY_NAME R3BLosDB)
Set(DEPENDENCIES Base)


GENERATE_LIBRARY()



