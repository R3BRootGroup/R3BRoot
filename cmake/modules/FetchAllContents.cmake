include(FetchContent)

if(FETCH_GTEST)
    FetchContent_Declare(
        googletest
        URL
            https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    FetchContent_MakeAvailable(googletest)
endif()

if(CMAKE_CXX_STANDARD GREATER_EQUAL 23)
FetchContent_Declare(
    glaze
    GIT_REPOSITORY https://github.com/stephenberry/glaze.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glaze)
endif()

# # fetch CLI11

# fetchcontent_declare(
#     cli11_proj
#     QUIET
#     GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
#     GIT_TAG v2.3.2)

# fetchcontent_makeavailable(cli11_proj)
