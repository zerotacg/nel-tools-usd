add_subdirectory(tests)


add_library(lib${PROJECT_NAME}-shape-to-usd)
add_library(${PROJECT_NAME}::lib::shape-to-usd ALIAS lib${PROJECT_NAME}-shape-to-usd)

target_compile_features(lib${PROJECT_NAME}-shape-to-usd PUBLIC cxx_std_20)

target_sources(lib${PROJECT_NAME}-shape-to-usd
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        src/convert.mpp
        src/mesh/Converter.mpp
        PRIVATE
        FILE_SET private_modules
        TYPE CXX_MODULES
        FILES
        src/mesh/ConverterCMesh.mpp
        PRIVATE
        src/convert.cpp
        src/mesh/Converter.cpp
        src/mesh/ConverterCMesh.cpp
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
        ${PROJECT_NAME}::format
)


set_target_properties(lib${PROJECT_NAME}-shape-to-usd
        PROPERTIES
            EXPORT_NAME lib-shape-to-usd
            OUTPUT_NAME "lib${PROJECT_NAME}-shape-to-usd"
)

install(
        TARGETS lib${PROJECT_NAME}-shape-to-usd
        COMPONENT "${PROJECT_NAME}"
)
