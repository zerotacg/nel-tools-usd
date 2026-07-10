add_test(NAME "usd-to-zone.help" COMMAND ${PROJECT_NAME}::usd-to-zone --help)

find_program(DIFF diff REQUIRED)


add_test(NAME usd-to-zone.write_zone
        COMMAND ${PROJECT_NAME}::usd-to-zone
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/zone.usda"
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/zone.received.zonel"
)

add_test(NAME usd-to-zone.write_zone_usda
        COMMAND ${PROJECT_NAME}::zone-to-usd
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/zone.received.zonel"
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/zone.received.usda"
)
set_tests_properties(usd-to-zone.write_zone_usda PROPERTIES DEPENDS usd-to-zone.write_zone)


add_test(NAME usd-to-zone.zone_matches_approval
        COMMAND ${DIFF}
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/zone.received.usda"
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/zone.approved.usda"
)
set_tests_properties(usd-to-zone.zone_matches_approval PROPERTIES DEPENDS usd-to-mesh.write_zone_usda)
