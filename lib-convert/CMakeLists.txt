add_subdirectory(tests)


add_library(${PROJECT_NAME}-convert STATIC)
add_library(${PROJECT_NAME}::convert ALIAS ${PROJECT_NAME}-convert)

target_compile_features(${PROJECT_NAME}-convert PUBLIC cxx_std_20)

target_sources(${PROJECT_NAME}-convert
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
        ${PROJECT_NAME}::format
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
