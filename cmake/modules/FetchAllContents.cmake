include(FetchContent)

if(FETCH_GTEST)
    fetchcontent_declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.0.zip
    )
    fetchcontent_makeavailable(googletest)

    message(STATUS "googletest_SOURCE_DIR = ${googletest_SOURCE_DIR}")

    if(EXISTS "${googletest_SOURCE_DIR}/CMakeLists.txt")
       file(STRINGS
         "${googletest_SOURCE_DIR}/CMakeLists.txt"
         GOOGLETEST_VERSION_LINE
         REGEX "set\\(GOOGLETEST_VERSION")

       message(STATUS "GOOGLETEST_VERSION line = ${GOOGLETEST_VERSION_LINE}")
    endif()

    if(TARGET gtest)
       message(STATUS "Target gtest FOUND")
    endif()
endif()
