cmake_minimum_required(VERSION 4.1..4.2)

project(learning-vcpkg VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(project_namespace "${PROJECT_NAME}")
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(version_config "${generated_dir}/${PROJECT_NAME}-config-version.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}-config.cmake")
set(package_components "")

find_package(fmt REQUIRED)
find_package(OpenAL REQUIRED)
find_package(ZLIB REQUIRED)

add_subdirectory(app-hello-world)
add_subdirectory(lib-example)


write_basic_package_version_file(
        "${version_config}" COMPATIBILITY SameMajorVersion
)

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in"
        "${project_config}"
        INSTALL_DESTINATION "${config_install_dir}"
)

install(
        FILES
        "${project_config}"
        "${version_config}"
        COMPONENT "${PROJECT_NAME}"
        DESTINATION "${config_install_dir}"
)

include(CPack)
