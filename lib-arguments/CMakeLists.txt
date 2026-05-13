add_library(${PROJECT_NAME}-arguments STATIC)
add_library(${PROJECT_NAME}::arguments ALIAS ${PROJECT_NAME}-arguments)

target_compile_features(${PROJECT_NAME}-arguments PUBLIC cxx_std_20)

target_sources(${PROJECT_NAME}-arguments
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        src/arguments.mpp
)

target_link_libraries(${PROJECT_NAME}-arguments
        PUBLIC
        NeL::misc
)


set_target_properties(${PROJECT_NAME}-arguments
        PROPERTIES
            EXPORT_NAME arguments
            OUTPUT_NAME "${PROJECT_NAME}-arguments"
)

install(
        TARGETS ${PROJECT_NAME}-arguments
        COMPONENT "${PROJECT_NAME}"
)
