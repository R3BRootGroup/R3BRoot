# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${R3BROOT_SOURCE_DIR}/field 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
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

GENERATE_LIBRARY()

