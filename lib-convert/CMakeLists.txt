add_subdirectory(tests)


add_library(${PROJECT_NAME}-convert STATIC)
add_library(${PROJECT_NAME}::convert ALIAS ${PROJECT_NAME}-convert)

target_compile_features(${PROJECT_NAME}-convert PUBLIC cxx_std_20)

target_sources(${PROJECT_NAME}-convert
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

target_link_libraries(${PROJECT_NAME}-convert
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

target_compile_definitions(${PROJECT_NAME}-convert PUBLIC $<$<CONFIG:Debug>:NL_DEBUG> $<$<CONFIG:Release>:NL_RELEASE>)


set_target_properties(${PROJECT_NAME}-convert
        PROPERTIES
            EXPORT_NAME convert
            OUTPUT_NAME "lib${PROJECT_NAME}-convert"
)

install(
        TARGETS ${PROJECT_NAME}-convert
        COMPONENT "${PROJECT_NAME}"
)
