add_library(nel-tools-lib-shape-to-usd STATIC)
add_library(${PROJECT_NAME}::shape-to-usd ALIAS nel-tools-lib-shape-to-usd)

target_sources(nel-tools-lib-shape-to-usd
        PUBLIC
        FILE_SET HEADERS
        FILES
        nel-tools/usd/format.h
        nel-tools/usd/convert/convert.h
        nel-tools/usd/mesh-converter/Converter.h
        nel-tools/usd/shape-to-usd/Settings.h
        PRIVATE
        nel-tools/usd/format.cpp
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
        EXPORT "${targets_export_name}"
        FILE_SET HEADERS
)

install(EXPORT "${targets_export_name}"
        COMPONENT "${PROJECT_NAME}"
        NAMESPACE "${project_namespace}::"
        DESTINATION "${config_install_dir}"
)

list(APPEND package_components "shape-to-usd")
set(package_components ${package_components} PARENT_SCOPE)
