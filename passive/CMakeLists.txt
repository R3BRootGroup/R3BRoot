# Create a library called "libPassive" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/r3bbase  
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/passive  )

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
R3BCave.cxx            
R3BGeoCave.cxx        
R3BGeoMagnet.cxx      
R3BGeoPassivePar.cxx  
R3BGeoPipe.cxx        
R3BGeoVacVesselCool.cxx        
R3BGeoTarget.cxx      
R3BMagnet.cxx         
R3BGladMagnet.cxx         
R3BPassiveContFact.cxx
R3BPipe.cxx           
R3BVacVesselCool.cxx           
R3BTarget.cxx 
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(LINKDEF PassiveLinkDef.h)
Set(LIBRARY_NAME R3BPassive) 
Set(DEPENDENCIES
    R3Bbase Field R3BPassive)

GENERATE_LIBRARY()
