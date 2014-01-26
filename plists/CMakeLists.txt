# Create a library called "libPlist" which includes the source files
# given in # the array . # The extension is already found.  Any number of
# sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
#put here all directories where header files are located
${BASE_INCLUDE_DIRECTORIES}
${R3BROOT_SOURCE_DIR}/passive
${R3BROOT_SOURCE_DIR}/mcstack
${R3BROOT_SOURCE_DIR}/tof
${R3BROOT_SOURCE_DIR}/plists
${GEANT4_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${GEANT4VMC_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
) 

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2")
set(CMAKE_CXX_FLAGS_RELEASE        "-O2")
set(CMAKE_CXX_FLAGS_DEBUG          "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
set(CMAKE_CXX_FLAGS_DEBUG          "-g")

link_directories( ${LINK_DIRECTORIES})

set(R3BPLIST_SRCS
G4EmHadronBuilder.cxx
G4EmMuonBuilder.cxx
G4EmQEDBuilder.cxx
G4LowEnergyQEDBuilder.cxx
G4PenelopeQEDBuilder.cxx
G4StepLimiterBuilder.cxx
G4StepLimiterMessenger.cxx
G4StepLimiterPerRegion.cxx
EmBinaryCascadeBuilder.cxx
EmGammaNucleusBuilder.cxx
EmHadronElasticBuilder.cxx
EmIonBinaryCascadeBuilder.cxx
R3BDecaysBuilder.cxx
R3BParticlesBuilder.cxx
R3BPhysicsList.cxx
R3BPhysicsListMessenger.cxx
R3BRunConfiguration.cxx  
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h R3BPLIST_HEADERS "${R3BPLIST_SRCS}")

set(R3BPLIST_DICT_HEADERS R3BRunConfiguration.h)
set(R3BPLIST_LINKDEF PlistLinkDef.h)
set(R3BPLIST_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/R3BPlistDict.cxx)

ROOT_GENERATE_DICTIONARY("${R3BPLIST_DICT_HEADERS}" "${R3BPLIST_LINKDEF}" "${R3BPLIST_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(R3BPLIST_SRCS ${R3BPLIST_SRCS} ${R3BPLIST_DICTIONARY})

add_library(R3BPlist SHARED ${R3BPLIST_SRCS})
target_link_libraries(R3BPlist ${ROOT_LIBRARIES})
set_target_properties(R3BPlist PROPERTIES 
${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS R3BPlist DESTINATION ${CMAKE_BINARY_DIR}/lib)
