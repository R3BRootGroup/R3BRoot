# Create a library called "R3BDB" 

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/r3bdb/commondb
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
R3BDBContFact.cxx
R3BDBRunInfo.cxx
R3BDBUnits.cxx
R3BDBPhysConsts.cxx
)

set(HEADERS
R3BDBContFact.h
R3BDBRunInfo.h
R3BDBUnits.h
R3BDBPhysConsts.h
${FAIRROOTPATH}/include/FairDbReader.h
${FAIRROOTPATH}/include/FairDbWriter.h
)


set(LINKDEF R3BDBLinkDef.h)
Set(LIBRARY_NAME R3BDB)


GENERATE_LIBRARY()



