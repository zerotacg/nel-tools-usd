#vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO ryzom/ryzomcore
        REF "core4"
        SHA512 ab6442b418b4135f50ad1bf69a67fa74f9bc0882f9ad2953e1f3e74fe48743e369fdd095f1a5c427e82addc2cc5ca394f22f929dc12f503f9fbf391ebe05b320
        HEAD_REF core4
        PATCHES
            0001-cmake-component-dependencies.patch
)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
        FEATURES
            3d          WITH_3D
            georges     WITH_GEORGES
            gui         WITH_GUI
            ligo        WITH_LIGO
            logic       WITH_LOGIC
            net         WITH_NET
            pacs        WITH_PACS
            sound       WITH_SOUND
            web         WITH_WEB
)

vcpkg_cmake_configure(
        SOURCE_PATH "${SOURCE_PATH}"
        OPTIONS
            ${FEATURE_OPTIONS}
            -DWITH_NEL=ON
            -DWITH_NEL_TESTS=OFF
            -DWITH_NEL_SAMPLES=OFF
            -DWITH_NEL_TOOLS=OFF
            -DWITH_DRIVER_OPENAL=OFF
            -DWITH_DRIVER_OPENGL=OFF
            -DWITH_DRIVER_OPENGL3=OFF
            -DWITH_NELNS=OFF
            -DWITH_LUA51=OFF
            -DWITH_LUA52=OFF
            -DWITH_RYZOM=OFF
            -DWITH_SNOWBALLS=OFF
            -DWITH_STUDIO=OFF
            -DWITH_TOOLS=OFF
            -DWITH_QT5=OFF
            -DWITH_LIBGSF=OFF
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(
        PACKAGE_NAME "NeL"
        CONFIG_PATH lib/cmake/NeL
)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
