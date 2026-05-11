add_test(NAME "usd-to-mesh.help" COMMAND ${PROJECT_NAME}::usd-to-mesh --help)

add_test(NAME usd-to-mesh.write_cube_shape
        COMMAND ${PROJECT_NAME}::usd-to-mesh
        --output-xml "${CMAKE_CURRENT_SOURCE_DIR}/approvals/cube.received.xml"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/cube.usda"
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/cube.received.shape"
)

add_test(NAME usd-to-mesh.write_cube_usda
        COMMAND ${PROJECT_NAME}::shape-to-usd
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/cube.received.shape"
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/cube.received.usda"
)
set_tests_properties(usd-to-mesh.write_cube_usda PROPERTIES DEPENDS usd-to-mesh.write_cube_shape)


find_program(DIFF diff REQUIRED)


add_test(NAME usd-to-mesh.cube_matches_approval
        COMMAND ${DIFF}
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/cube.received.usda"
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/cube.approved.usda"
)
set_tests_properties(usd-to-mesh.cube_matches_approval PROPERTIES DEPENDS usd-to-mesh.write_cube_usda)
