# Create a library called "libPlist" which includes the source files
# given in # the array . # The extension is already found.  Any number of
# sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES 
${SYSTEM_INCLUDE_DIRECTORIES}
${BASE_INCLUDE_DIRECTORIES}
${GEANT4_INCLUDE_DIR}
${GEANT4VMC_INCLUDE_DIR}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/passive
${R3BROOT_SOURCE_DIR}/tof
${R3BROOT_SOURCE_DIR}/plists
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${GEANT4_LIBRARY_DIR}
${GEANT4VMC_LIBRARY_DIR}
) 

#set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2")
#set(CMAKE_CXX_FLAGS_RELEASE        "-O2")
#set(CMAKE_CXX_FLAGS_DEBUG          "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
#set(CMAKE_CXX_FLAGS_DEBUG          "-g")

link_directories( ${LINK_DIRECTORIES})

set(SRCS
G4EmHadronBuilder.cxx
G4EmMuonBuilder.cxx
G4EmQEDBuilder.cxx
G4LowEnergyQEDBuilder.cxx
G4PenelopeQEDBuilder.cxx
G4StepLimiterBuilder.cxx
G4StepLimiterMessenger.cxx
G4StepLimiterPerRegion.cxx
#EmBinaryCascadeBuilder.cxx
#EmGammaNucleusBuilder.cxx
#EmHadronElasticBuilder.cxx
#EmIonBinaryCascadeBuilder.cxx
R3BDecaysBuilder.cxx
R3BParticlesBuilder.cxx
R3BPhysicsList.cxx
R3BPhysicsListMessenger.cxx
R3BRunConfiguration.cxx  
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

Set(DEPENDENCIES
    VMC G4physicslists G4vis_management geant4vmc)

Set(LIBRARY_NAME R3BPlist)

GENERATE_LIBRARY()

