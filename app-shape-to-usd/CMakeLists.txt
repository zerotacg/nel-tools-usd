add_library(nel-tools-lib-shape-to-usd STATIC)
add_library(${PROJECT_NAME}::shape-to-usd ALIAS nel-tools-lib-shape-to-usd)

target_compile_features(nel-tools-lib-shape-to-usd PUBLIC cxx_std_20)

target_sources(nel-tools-lib-shape-to-usd
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        nel-tools/usd/convert/convert.mpp
        nel-tools/usd/format.mpp
        nel-tools/usd/mesh-converter/Converter.mpp
        PRIVATE
        FILE_SET private_modules
        TYPE CXX_MODULES
        FILES
        nel-tools/usd/mesh-converter/ConverterCMesh.mpp
        PRIVATE
        nel-tools/usd/convert/convert.cpp
        nel-tools/usd/mesh-converter/Converter.cpp
        nel-tools/usd/mesh-converter/ConverterCMesh.cpp
)

target_link_libraries(nel-tools-lib-shape-to-usd
        PUBLIC
        fmt::fmt
        NeL::3d
        NeL::misc
        pxr::usd
        pxr::ar
        pxr::Geom
        pxr::Hydra
        pxr::Shade
)

target_compile_definitions(nel-tools-lib-shape-to-usd PUBLIC $<$<CONFIG:Debug>:NL_DEBUG> $<$<CONFIG:Release>:NL_RELEASE>)

add_executable(nel-tools-shape-to-usd)
add_executable(${PROJECT_NAME}::app::shape-to-usd ALIAS nel-tools-shape-to-usd)

target_sources(nel-tools-shape-to-usd
        PRIVATE
        FILE_SET CXX_MODULES
        FILES
        nel-tools/usd/shape-to-usd/Settings.mpp
        PRIVATE
        nel-tools/usd/shape-to-usd/main.cpp
)

target_link_libraries(nel-tools-shape-to-usd
        PRIVATE
        ${PROJECT_NAME}::shape-to-usd
)
target_compile_features(nel-tools-shape-to-usd PRIVATE cxx_std_20)


add_test(NAME "${PROJECT_NAME}.can-start" COMMAND nel-tools-shape-to-usd --help)


add_subdirectory(tests)


set_target_properties(nel-tools-shape-to-usd
        PROPERTIES
            EXPORT_NAME shape-to-usd
            OUTPUT_NAME "${PROJECT_NAME}-shape-to-usd"
)

set(targets_export_name "${PROJECT_NAME}-targets-shape-to-usd")

install(
        TARGETS nel-tools-shape-to-usd
        COMPONENT "${PROJECT_NAME}"
)

list(APPEND package_components "shape-to-usd")
set(package_components ${package_components} PARENT_SCOPE)
