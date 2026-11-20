add_library(R3BMille SHARED)

target_sources(
    R3BMille
    PRIVATE Mille.cxx ParResultReader.cxx PedeLauncher.cxx SteerWriter.cxx
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
            FILES
                MilleEntry.h
                Mille.h
                MilleLinkDef.h
                ParResultReader.h
                PedeLauncher.h
                SteerWriter.h
)

target_include_directories(R3BMille PUBLIC ${Boost_INCLUDE_DIR})

target_link_libraries(
    R3BMille
    PUBLIC Boost::headers ROOT::Core
    PRIVATE fmt::fmt range-v3::range-v3 magic_enum::magic_enum glaze::glaze
)
target_compile_definitions(R3BMille PRIVATE BOOST_PROCESS_USE_STD_FS=1)
if(WITH_MILLEPEDE)
    target_compile_definitions(R3BMille PUBLIC WITH_MILLEPEDE)
endif()

root_generate_dictionary(
    R3BMille_dict
    MilleEntry.h
    MODULE
    R3BMille
    LINKDEF
    MilleLinkDef.h
)

install(TARGETS R3BMille EXPORT R3BRootLibs FILE_SET HEADERS)
