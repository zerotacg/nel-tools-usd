add_subdirectory(tests)


add_library(lib${PROJECT_NAME}-zone-to-usd)
add_library(${PROJECT_NAME}::lib::zone-to-usd ALIAS lib${PROJECT_NAME}-zone-to-usd)
target_compile_features(lib${PROJECT_NAME}-zone-to-usd PUBLIC cxx_std_20)
target_sources(lib${PROJECT_NAME}-zone-to-usd
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        src/paths.mpp
        src/Settings.mpp
        src/tokens.mpp
)
target_link_libraries(lib${PROJECT_NAME}-zone-to-usd
        PUBLIC
        fmt::fmt
        NeL::3d
        NeL::misc
        pxr::usd
        pxr::ar
        pxr::Geom
        pxr::Hydra
        pxr::Shade
        ${PROJECT_NAME}::arguments
        ${PROJECT_NAME}::common
        ${PROJECT_NAME}::format
)


add_executable(${PROJECT_NAME}-zone-to-usd)
add_executable(${PROJECT_NAME}::zone-to-usd ALIAS ${PROJECT_NAME}-zone-to-usd)

target_sources(${PROJECT_NAME}-zone-to-usd
        PRIVATE
        src/main.cpp
)

target_link_libraries(${PROJECT_NAME}-zone-to-usd
        PRIVATE
        ${PROJECT_NAME}::lib::zone-to-usd
)
target_compile_features(${PROJECT_NAME}-zone-to-usd PRIVATE cxx_std_20)


set_target_properties(${PROJECT_NAME}-zone-to-usd
        PROPERTIES
        EXPORT_NAME zone-to-usd
        OUTPUT_NAME "${PROJECT_NAME}-zone-to-usd"
)

set(targets_export_name "${PROJECT_NAME}-targets-zone-to-usd")

install(
        TARGETS ${PROJECT_NAME}-zone-to-usd
        RUNTIME_DEPENDENCIES
        COMPONENT "${PROJECT_NAME}"
)

list(APPEND package_components "zone-to-usd")
set(package_components ${package_components} PARENT_SCOPE)
