cmake_minimum_required(VERSION 4.1..4.2)

project(nel-tools-usd VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
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
set(INSTALL_GTEST OFF)

find_package(fmt REQUIRED)
find_package(pxr REQUIRED)
find_package(NeL REQUIRED COMPONENTS misc)
find_package(GTest REQUIRED)

if(NOT TARGET pxr::usd)
    add_library(pxr::ar ALIAS ar)
    add_library(pxr::usd ALIAS usd)
    add_library(pxr::Geom ALIAS usdGeom)
    add_library(pxr::Hydra ALIAS usdHydra)
    add_library(pxr::Shade ALIAS usdShade)
endif ()


add_subdirectory(app-shape-to-usd)


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
