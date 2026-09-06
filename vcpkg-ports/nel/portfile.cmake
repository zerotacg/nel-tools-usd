#vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO ryzom/ryzomcore
        REF "core4"
        SHA512 eaf9c6d460047d132ce0fe7c2deb2b1a358422637863d8351a914ef52cc8a5297e8ae53570ca8ca3a138acaf1a562ee6c1deec12c6665e3d258e51288b14b436
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

string(COMPARE EQUAL "${VCPKG_LIBRARY_LINKAGE}" "static" BUILD_STATIC)
string(COMPARE EQUAL "${VCPKG_LIBRARY_LINKAGE}" "dynamic" BUILD_SHARED)

vcpkg_cmake_configure(
        SOURCE_PATH "${SOURCE_PATH}"
        OPTIONS
            ${FEATURE_OPTIONS}
            -DWITH_STATIC=${BUILD_STATIC}
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
vcpkg_fixup_pkgconfig()
vcpkg_copy_pdbs()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
