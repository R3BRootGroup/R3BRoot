# Create a library called "R3BDB" 

Set(SYSTEM_INCLUDE_DIRECTORIES 
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
${R3BROOT_SOURCE_DIR}/r3bdb/commondb
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
R3BDBContFact.cxx
R3BDBRunInfo.cxx
R3BDBLmdFileInfo.cxx
R3BDBDaqInfo.cxx
R3BDBUnits.cxx
R3BDBPhysConsts.cxx
R3BDBLmdAnalyzer.cxx
R3BDBLmdUnpack.cxx
)

set(HEADERS
R3BDBContFact.h
R3BDBRunInfo.h
R3BDBLmdFileInfo.h
R3BDBDaqInfo.h
R3BDBUnits.h
R3BDBPhysConsts.h
R3BDBLmdAnalyzer.h
R3BDBLmdUnpack.h
${FAIRROOTPATH}/include/FairDbReader.h
${FAIRROOTPATH}/include/FairDbWriter.h
)


set(LINKDEF R3BDBLinkDef.h)
set(LIBRARY_NAME R3BDB)
set(DEPENDENCIES
    R3Bbase R3BData R3BPassive)

GENERATE_LIBRARY()



