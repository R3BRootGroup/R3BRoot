#Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wformat-security -march=native")

set(SYSTEM_INCLUDE_DIRECTORIES
        ${SYSTEM_INCLUDE_DIRECTORIES}
        ${BASE_INCLUDE_DIRECTORIES}
        )
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(INCLUDE_DIRECTORIES
        ${R3BROOT_SOURCE_DIR}/r3bdata
        ${R3BROOT_SOURCE_DIR}/r3bdata/neulandData
        ${R3BROOT_SOURCE_DIR}/r3bbase
        ${R3BROOT_SOURCE_DIR}/neuland/shared
        )

link_directories(
        ${ROOT_LIBRARY_DIR}
        ${Boost_LIBRARY_DIRS}
        ${FAIRROOT_LIBRARY_DIR}
)

add_subdirectory(clustering)
add_subdirectory(digitizing)
add_subdirectory(reconstruction)
add_subdirectory(shared)
add_subdirectory(simulation)
add_subdirectory(test)
add_subdirectory(unpack)
