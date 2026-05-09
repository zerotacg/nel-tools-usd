cmake_minimum_required(VERSION 4.1..4.2)

project(nel-tools-usd VERSION 1.0.0 LANGUAGES CXX)

enable_testing()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

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
add_subdirectory(lib-convert)


include(CPack)
