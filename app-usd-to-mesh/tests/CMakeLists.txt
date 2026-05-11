add_test(NAME "usd-to-mesh.help" COMMAND ${PROJECT_NAME}::usd-to-mesh --help)

add_test(NAME "usd-to-mesh.write_cube"
        COMMAND ${PROJECT_NAME}::usd-to-mesh
        --output-xml "${CMAKE_CURRENT_SOURCE_DIR}/approvals/cube.received.xml"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/cube.usda"
        "${CMAKE_CURRENT_BINARY_DIR}/cube.shape"
)


find_program(DIFF diff REQUIRED)


add_test(NAME "usd-to-mesh.cube_matches_approval"
        COMMAND ${DIFF}
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/cube.received.xml"
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/cube.approved.xml"
)
set_tests_properties("usd-to-mesh.cube_matches_approval" PROPERTIES DEPENDS "usd-to-mesh.write_cube")
