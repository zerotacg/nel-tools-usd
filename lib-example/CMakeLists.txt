add_library(learning-vcpkg-example)
add_library(${PROJECT_NAME}::example ALIAS learning-vcpkg-example)

target_sources(learning-vcpkg-example
        PUBLIC
        FILE_SET HEADERS
        FILES
        learning-vcpkg/example/example.hpp
        PRIVATE
        learning-vcpkg/example/example.cpp
)

target_precompile_headers(learning-vcpkg-hello-world
        PUBLIC
        learning-vcpkg/example/example.hpp
)

target_link_libraries(learning-vcpkg-example
        PRIVATE fmt::fmt
)


set_target_properties(learning-vcpkg-example PROPERTIES EXPORT_NAME example)
set_target_properties(learning-vcpkg-example PROPERTIES OUTPUT_NAME "${PROJECT_NAME}-example")

set(targets_export_name "${PROJECT_NAME}-targets-example")

install(TARGETS learning-vcpkg-example
        COMPONENT "${PROJECT_NAME}"
        EXPORT "${targets_export_name}"
        FILE_SET HEADERS
)

install(EXPORT "${targets_export_name}"
        COMPONENT "${PROJECT_NAME}"
        NAMESPACE "${project_namespace}::"
        DESTINATION "${config_install_dir}"
)

list(APPEND package_components "example")
set(package_components ${package_components} PARENT_SCOPE)
