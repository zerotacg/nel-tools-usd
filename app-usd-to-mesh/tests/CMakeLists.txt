add_test(NAME "usd-to-mesh.can-start" COMMAND ${PROJECT_NAME}::usd-to-mesh --help)

add_test(NAME "usd-to-mesh.cube"
        COMMAND ${PROJECT_NAME}::usd-to-mesh
        --output-xml "${CMAKE_CURRENT_BINARY_DIR}/cube.received.xml"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/cube.usda"
        "${CMAKE_CURRENT_BINARY_DIR}/cube.shape"
)
