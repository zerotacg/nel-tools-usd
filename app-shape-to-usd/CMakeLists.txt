add_subdirectory(tests)

add_executable(${PROJECT_NAME}-shape-to-usd)
add_executable(${PROJECT_NAME}::shape-to-usd ALIAS ${PROJECT_NAME}-shape-to-usd)

target_sources(${PROJECT_NAME}-shape-to-usd
        PRIVATE
        FILE_SET CXX_MODULES
        FILES
        src/Settings.mpp
        PRIVATE
        src/main.cpp
)

target_link_libraries(${PROJECT_NAME}-shape-to-usd
        PRIVATE
        ${PROJECT_NAME}::arguments
        ${PROJECT_NAME}::convert
)
target_compile_features(${PROJECT_NAME}-shape-to-usd PRIVATE cxx_std_20)


set_target_properties(${PROJECT_NAME}-shape-to-usd
        PROPERTIES
        EXPORT_NAME shape-to-usd
        OUTPUT_NAME "${PROJECT_NAME}-shape-to-usd"
)

set(targets_export_name "${PROJECT_NAME}-targets-shape-to-usd")

install(
        TARGETS ${PROJECT_NAME}-shape-to-usd
        COMPONENT "${PROJECT_NAME}"
)

list(APPEND package_components "shape-to-usd")
set(package_components ${package_components} PARENT_SCOPE)
