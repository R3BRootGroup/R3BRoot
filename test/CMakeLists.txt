if(GTest_FOUND)
    include(GoogleTest)
    set(TEST_SOURCE_FILES)

    add_subdirectory(r3bbase)

    add_executable(R3BUnitTests ${TEST_SOURCE_FILES} R3BUnitTests.cxx)
    target_link_libraries(R3BUnitTests PRIVATE GTest::gtest_main GTest::gmock_main R3BBase)
    gtest_discover_tests(R3BUnitTests DISCOVERY_TIMEOUT 600)
endif(GTest_FOUND)
