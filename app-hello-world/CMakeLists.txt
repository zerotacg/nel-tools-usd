add_executable(learning-vcpkg-hello-world)
add_executable(${PROJECT_NAME}::hello-world ALIAS learning-vcpkg-hello-world)

target_sources(learning-vcpkg-hello-world
        PRIVATE
        learning-vcpkg/hello-world/main.cpp
)

target_link_libraries(learning-vcpkg-hello-world
        PRIVATE
        learning-vcpkg::example
        fmt::fmt
        OpenAL::OpenAL
)

add_test(NAME "${PROJECT_NAME}.can-start" COMMAND learning-vcpkg-hello-world)


set_target_properties(learning-vcpkg-hello-world PROPERTIES EXPORT_NAME hello-world)
set_target_properties(learning-vcpkg-hello-world PROPERTIES OUTPUT_NAME "${PROJECT_NAME}-hello-world")

set(targets_export_name "${PROJECT_NAME}-targets-hello-world")

install(
        TARGETS learning-vcpkg-hello-world
        COMPONENT "${PROJECT_NAME}"
        EXPORT "${targets_export_name}"
        FILE_SET HEADERS
)

install(EXPORT "${targets_export_name}"
        COMPONENT "${PROJECT_NAME}"
        NAMESPACE "${project_namespace}::"
        DESTINATION "${config_install_dir}"
)

list(APPEND package_components "hello-world")
set(package_components ${package_components} PARENT_SCOPE)
