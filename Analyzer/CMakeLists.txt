# LLVM requires CMake >= 3.4.3
CMAKE_MINIMUM_REQUIRED(VERSION 3.4.3)
project(llvm-tutor-hello-world)

# Step 1. Make sure that LLVMConfig.cmake _is_ on CMake's search path
# set(LT_LLVM_INSTALL_DIR "" CACHE PATH "LLVM installation directory")
# set(LT_LLVM_CMAKE_CONFIG_DIR "${LT_LLVM_INSTALL_DIR}/lib/cmake/llvm/")
# list(APPEND CMAKE_PREFIX_PATH "${LT_LLVM_CMAKE_CONFIG_DIR}")

set(CMAKE_PREFIX_PATH "/home/joshua/llvm-project/build/lib/cmake/llvm")

# Step 2. Load LLVM config from ... LLVMConfig.cmake
find_package(LLVM REQUIRED CONFIG)

# Analyzer includes header files from LLVM
include_directories(${LLVM_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 14 CACHE STRING "")

if(NOT LLVM_ENABLE_RTTI)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

# Step 3. Define the plugin/pass/library
# Gotcha: You don't need to use add_llvm_library
add_library(Analyzer SHARED analyzer.cpp)

target_link_libraries(Analyzer)