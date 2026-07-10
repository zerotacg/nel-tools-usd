add_subdirectory(tests)


add_library(lib${PROJECT_NAME}-usd-to-zone)
add_library(${PROJECT_NAME}::lib::usd-to-zone ALIAS lib${PROJECT_NAME}-usd-to-zone)
target_compile_features(lib${PROJECT_NAME}-usd-to-zone PUBLIC cxx_std_20)
target_sources(lib${PROJECT_NAME}-usd-to-zone
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        src/paths.mpp
        src/tokens.mpp
)
target_link_libraries(lib${PROJECT_NAME}-usd-to-zone
        PUBLIC
        fmt::fmt
        NeL::3d
        NeL::misc
        pxr::usd
        pxr::ar
        pxr::Geom
        pxr::Hydra
        pxr::Shade
        ${PROJECT_NAME}::common
        ${PROJECT_NAME}::format
)


add_executable(${PROJECT_NAME}-usd-to-zone)
add_executable(${PROJECT_NAME}::usd-to-zone ALIAS ${PROJECT_NAME}-usd-to-zone)

target_sources(${PROJECT_NAME}-usd-to-zone
        PRIVATE
        FILE_SET CXX_MODULES
        FILES
        src/Settings.mpp
        PRIVATE
        src/main.cpp
)

target_link_libraries(${PROJECT_NAME}-usd-to-zone
        PRIVATE
        ${PROJECT_NAME}::arguments
        ${PROJECT_NAME}::lib::usd-to-zone
)
target_compile_features(${PROJECT_NAME}-usd-to-zone PRIVATE cxx_std_20)


set_target_properties(${PROJECT_NAME}-usd-to-zone
        PROPERTIES
        EXPORT_NAME usd-to-zone
        OUTPUT_NAME "${PROJECT_NAME}-usd-to-zone"
)

set(targets_export_name "${PROJECT_NAME}-targets-usd-to-zone")

install(
        TARGETS ${PROJECT_NAME}-usd-to-zone
        RUNTIME_DEPENDENCIES
        COMPONENT "${PROJECT_NAME}"
)

list(APPEND package_components "usd-to-zone")
set(package_components ${package_components} PARENT_SCOPE)
