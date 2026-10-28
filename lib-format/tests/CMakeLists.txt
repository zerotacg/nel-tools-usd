add_executable(format.test)
target_sources(format.test
        PRIVATE
        format.test.cpp
)
target_link_libraries(format.test
        PRIVATE
        GTest::gtest_main
        GTest::gmock
        ${PROJECT_NAME}::format
)
target_compile_features(format.test PRIVATE cxx_std_20)
add_test(NAME "format.format" COMMAND "format.test")
