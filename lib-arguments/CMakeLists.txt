add_library(${PROJECT_NAME}-arguments STATIC)
add_library(${PROJECT_NAME}::arguments ALIAS ${PROJECT_NAME}-arguments)

target_compile_features(${PROJECT_NAME}-arguments PUBLIC cxx_std_20)

target_sources(${PROJECT_NAME}-arguments
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        nel-tools/usd/arguments.mpp
)

target_link_libraries(${PROJECT_NAME}-arguments
        PUBLIC
        NeL::misc
)

target_compile_definitions(${PROJECT_NAME}-arguments PUBLIC $<$<CONFIG:Debug>:NL_DEBUG> $<$<CONFIG:Release>:NL_RELEASE>)


set_target_properties(${PROJECT_NAME}-arguments
        PROPERTIES
            EXPORT_NAME arguments
            OUTPUT_NAME "lib${PROJECT_NAME}-arguments"
)

install(
        TARGETS ${PROJECT_NAME}-arguments
        COMPONENT "${PROJECT_NAME}"
)
