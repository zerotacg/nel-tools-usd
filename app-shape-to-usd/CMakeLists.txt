add_subdirectory(tests)


add_library(lib${PROJECT_NAME}-shape-to-usd)
add_library(${PROJECT_NAME}::lib::shape-to-usd ALIAS lib${PROJECT_NAME}-shape-to-usd)
target_compile_features(lib${PROJECT_NAME}-shape-to-usd PUBLIC cxx_std_20)
target_sources(lib${PROJECT_NAME}-shape-to-usd
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        src/convert/material/material.mpp
        src/convert/material/TextureSettings.mpp
        src/convert/convert.mpp
        src/convert/mesh/Converter.mpp
        src/paths.mpp
        src/tokens.mpp
        PRIVATE
        FILE_SET private_modules
        TYPE CXX_MODULES
        FILES
        src/convert/mesh/ConverterCMesh.mpp
        src/convert/mesh/ConverterCMeshMRMSkinned.mpp
        PRIVATE
        src/convert/material/material.cpp
        src/convert/convert.cpp
        src/convert/mesh/Converter.cpp
        src/convert/mesh/ConverterCMesh.cpp
        src/convert/mesh/ConverterCMeshMRMSkinned.cpp
)
target_link_libraries(lib${PROJECT_NAME}-shape-to-usd
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
        ${PROJECT_NAME}::lib::shape-to-usd
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
        RUNTIME_DEPENDENCIES
        COMPONENT "${PROJECT_NAME}"
)

list(APPEND package_components "shape-to-usd")
set(package_components ${package_components} PARENT_SCOPE)
