set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_VERSION 11) # Adjust to match the macOS target version
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14) # Set minimum macOS version

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_ASM_COMPILER clang)
set(CMAKE_AR /usr/local/cctools/bin/ar)
set(CMAKE_RANLIB /usr/local/cctools/bin/ranlib)
set(CMAKE_STRIP /usr/local/cctools/bin/ranlib)
set(CMAKE_NM /usr/local/cctools/bin/nm)
set(CMAKE_LINKER /usr/local/cctools/bin/ld)

set(CMAKE_C_COMPILER_TARGET x86_64-apple-darwin-macho) # Change to aarch64-apple-darwin for ARM
set(CMAKE_CXX_COMPILER_TARGET x86_64-apple-darwin-macho)
set(CMAKE_ASM_COMPILER_TARGET x86_64-apple-darwin-macho)

set(CMAKE_SYSROOT /home/daniel/Pulled/MacOSX-SDKs/MacOSX11.3.sdk)
set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=/usr/local/cctools/bin/ld -Wno-unused-command-line-argument")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=/usr/local/cctools/bin/ld -Wno-unused-command-line-argument")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
