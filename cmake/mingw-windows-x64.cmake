set(CMAKE_SYSTEM_NAME Windows)

# specify the cross compiler
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# where is the target environment
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32;${CMAKE_FIND_ROOT_PATH}) # workaround for check in vcpkg's zlib port

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

# alternative to chaining the toolchain via VCPKG_CHAINLOAD_TOOLCHAIN_FILE
# also see https://learn.microsoft.com/en-us/vcpkg/users/buildsystems/cmake-integration#using-multiple-toolchain-files
#vcpkg
#set(MINGW TRUE)
#set(VCPKG_TARGET_TRIPLET x64-mingw-static)
#include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
