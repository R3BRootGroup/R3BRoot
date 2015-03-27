# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES 
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
${R3BROOT_SOURCE_DIR}/field 
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
R3BFieldConst.cxx     
R3BFieldContFact.cxx    
R3BFieldMap.cxx
R3BFieldPar.cxx          
R3BFieldCreator.cxx
R3BGladFieldMap.cxx
R3BFieldInterp.cxx
R3BAladinFieldMap.cxx  )

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF FieldLinkDef.h)
set(LIBRARY_NAME Field)
set(DEPENDENCIES
    Base)

GENERATE_LIBRARY()

