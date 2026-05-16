add_test(NAME "shape-to-usd.help" COMMAND ${PROJECT_NAME}::shape-to-usd --help)

add_test(NAME "shape-to-usd.write_cube_usda"
        COMMAND ${PROJECT_NAME}::shape-to-usd
        --texture-file-to-lower-case
        --texture-file-replace-extension texture.png
        --texture-file-prefix "textures/"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/cube.shape"
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/cube.received.usda"
)


find_program(DIFF_PROGRAM diff REQUIRED)


add_test(NAME "shape-to-usd.cube_matches_approval"
        COMMAND ${DIFF_PROGRAM}
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/cube.received.usda"
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/cube.approved.usda"
)
set_tests_properties("shape-to-usd.cube_matches_approval" PROPERTIES DEPENDS "shape-to-usd.write_cube_usda")


add_executable(shape-to-usd.convert.test)
target_sources(shape-to-usd.convert.test
        PRIVATE
        convert.test.cpp
)
target_link_libraries(shape-to-usd.convert.test
        PRIVATE
        GTest::gtest_main
        GTest::gmock
        ${PROJECT_NAME}::lib::shape-to-usd
)
target_compile_features("shape-to-usd.convert.test" PRIVATE cxx_std_20)
add_test(NAME "shape-to-usd.convert" COMMAND "shape-to-usd.convert.test")
