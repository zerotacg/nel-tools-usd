add_subdirectory(tests)

add_executable(${PROJECT_NAME}-usd-to-mesh)
add_executable(${PROJECT_NAME}::usd-to-mesh ALIAS ${PROJECT_NAME}-usd-to-mesh)

target_sources(${PROJECT_NAME}-usd-to-mesh
        PRIVATE
        FILE_SET CXX_MODULES
        FILES
        src/convert/convert.mpp
        src/convert/Converter.mpp
        src/Settings.mpp
        PRIVATE
        src/convert/convert.cpp
        src/convert/Converter.cpp
        src/main.cpp
)

target_link_libraries(${PROJECT_NAME}-usd-to-mesh
        PRIVATE
        ${PROJECT_NAME}::convert
)
target_compile_features(${PROJECT_NAME}-usd-to-mesh PRIVATE cxx_std_20)


set_target_properties(${PROJECT_NAME}-usd-to-mesh
        PROPERTIES
        EXPORT_NAME usd-to-mesh
        OUTPUT_NAME "${PROJECT_NAME}-usd-to-mesh"
)

set(targets_export_name "${PROJECT_NAME}-targets-usd-to-mesh")

install(
        TARGETS ${PROJECT_NAME}-usd-to-mesh
        COMPONENT "${PROJECT_NAME}"
)

list(APPEND package_components "usd-to-mesh")
set(package_components ${package_components} PARENT_SCOPE)
