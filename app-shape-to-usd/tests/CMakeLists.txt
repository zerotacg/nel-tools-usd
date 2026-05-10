add_test(NAME "shape-to-usd.help" COMMAND ${PROJECT_NAME}::shape-to-usd --help)

add_test(NAME "shape-to-usd.write_cube"
        COMMAND ${PROJECT_NAME}::shape-to-usd
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/cube.shape"
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/cube.received.usda"
)


find_program(DIFF_PROGRAM diff REQUIRED)


add_test(NAME "shape-to-usd.cube_matches_approval"
        COMMAND ${DIFF_PROGRAM}
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/cube.received.usda"
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/cube.approved.usda"
)
set_tests_properties("shape-to-usd.cube_matches_approval" PROPERTIES DEPENDS "shape-to-usd.write_cube")
