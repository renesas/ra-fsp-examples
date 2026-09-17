cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(sample_app_cpp)

set(TVM_ROOT $ENV{TVM_HOME})
include_directories(${TVM_ROOT}/include)
include_directories(${TVM_ROOT}/3rdparty/dlpack/include)
include_directories(${TVM_ROOT}/3rdparty/dmlc-core/include)
include_directories(${TVM_ROOT}/3rdparty/compiler-rt)

set(TVM_RUNTIME_LIB ${TVM_ROOT}/build_runtime/libtvm_runtime.so)
set(SRC sample_app.cpp MeraDrpRuntimeWrapper.cpp PreRuntime.cpp)
set(EXE_NAME sample_app)

add_executable(${EXE_NAME} ${SRC})
target_link_libraries(${EXE_NAME} ${TVM_RUNTIME_LIB})
