add_library(${PROJECT_NAME}-format STATIC)
add_library(${PROJECT_NAME}::format ALIAS ${PROJECT_NAME}-format)

target_compile_features(${PROJECT_NAME}-format PUBLIC cxx_std_20)

target_sources(${PROJECT_NAME}-format
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        nel-tools/usd/format.mpp
)

target_link_libraries(${PROJECT_NAME}-format
        PUBLIC
        fmt::fmt
)

target_compile_definitions(${PROJECT_NAME}-format PUBLIC $<$<CONFIG:Debug>:NL_DEBUG> $<$<CONFIG:Release>:NL_RELEASE>)


set_target_properties(${PROJECT_NAME}-format
        PROPERTIES
            EXPORT_NAME format
            OUTPUT_NAME "lib${PROJECT_NAME}-format"
)

install(
        TARGETS ${PROJECT_NAME}-format
        COMPONENT "${PROJECT_NAME}"
)
