add_library(${PROJECT_NAME}-common STATIC)
add_library(${PROJECT_NAME}::common ALIAS ${PROJECT_NAME}-common)

target_compile_features(${PROJECT_NAME}-common PUBLIC cxx_std_20)

target_sources(${PROJECT_NAME}-common
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        src/common.mpp
)

target_link_libraries(${PROJECT_NAME}-common
        PUBLIC
        pxr::usd
)


set_target_properties(${PROJECT_NAME}-common
        PROPERTIES
            EXPORT_NAME common
            OUTPUT_NAME "lib${PROJECT_NAME}-common"
)

install(
        TARGETS ${PROJECT_NAME}-common
        COMPONENT "${PROJECT_NAME}"
)
