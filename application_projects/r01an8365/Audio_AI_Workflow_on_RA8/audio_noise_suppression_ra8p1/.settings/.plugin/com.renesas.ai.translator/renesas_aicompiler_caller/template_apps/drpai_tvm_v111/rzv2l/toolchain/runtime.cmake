set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(MERA_DRP_RUNTIME ON)
set(DCMAKE_SYSTEM_VERSION 1)

set(CMAKE_SYSROOT $ENV{SDK}/sysroots/aarch64-poky-linux)
set(CMAKE_FIND_ROOT_PATH $ENV{SDK}/sysroots/aarch64-poky-linux/usr/include/gnu)
set(CMAKE_CXX_COMPILER $ENV{SDK}/sysroots/x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/aarch64-poky-linux-g++)
set(CMAKE_C_COMPILER $ENV{SDK}/sysroots/x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/aarch64-poky-linux-gcc)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
