include(FetchContent)

if(FETCH_GTEST)
    fetchcontent_declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    fetchcontent_makeavailable(googletest)
endif()
# fetch CLI11

fetchcontent_declare(
    cli11_proj
    QUIET
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.3.2)

fetchcontent_makeavailable(cli11_proj)
