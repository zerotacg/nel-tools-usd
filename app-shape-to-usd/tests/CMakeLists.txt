add_executable(convert_face_indices_test)

target_sources(convert_face_indices_test
        PRIVATE
        convert_face_indices.test.cpp
)

target_link_libraries(convert_face_indices_test
        PRIVATE
        GTest::gtest_main
        GTest::gmock
)

add_test(NAME "convert_face_indices_test" COMMAND "convert_face_indices_test")
target_compile_features("convert_face_indices_test" PRIVATE cxx_std_17)
