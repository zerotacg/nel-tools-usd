add_executable(convert_test)

target_sources(convert_test
        PRIVATE
        convert.test.cpp
)

target_link_libraries(convert_test
        PRIVATE
        GTest::gtest_main
        GTest::gmock
        ${PROJECT_NAME}::shape-to-usd
)

add_test(NAME "convert_test" COMMAND "convert_test")
target_compile_features("convert_test" PRIVATE cxx_std_20)
