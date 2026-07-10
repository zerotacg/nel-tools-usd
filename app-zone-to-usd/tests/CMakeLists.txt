add_test(NAME "zone-to-usd.help" COMMAND ${PROJECT_NAME}::zone-to-usd --help)

find_program(DIFF_PROGRAM diff REQUIRED)

add_test(NAME "zone-to-usd.write_zone_usda"
        COMMAND ${PROJECT_NAME}::zone-to-usd
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/zone.zonel"
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/zone.received.usda"
)



add_test(NAME "zone-to-usd.zone_matches_approval"
        COMMAND ${DIFF_PROGRAM}
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/zone.received.usda"
        "${CMAKE_CURRENT_SOURCE_DIR}/approvals/zone.approved.usda"
)
set_tests_properties("zone-to-usd.zone_matches_approval" PROPERTIES DEPENDS "zone-to-usd.write_zone_usda")
