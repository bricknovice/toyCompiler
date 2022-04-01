CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

PROJECT(toyCompiler)

SET(CMAKE_CXX_STANDARD 11)
find_package(Threads REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

BISON_TARGET(parser parser.y parser.cpp
                DEFINES_FILE parser.hpp)
FLEX_TARGET(lexer tokens.l tokens.cpp)
ADD_FLEX_BISON_DEPENDENCY(lexer parser)

llvm_map_components_to_libnames(llvm_libs system-libs)

add_executable(toyCompiler ${BISON_parser_OUTPUTS} ${FLEX_lexer_OUTPUTS} codegen.cpp codegen.h node.h main.cpp)
target_compile_options(toyCompiler PRIVATE -Wall)
target_link_libraries(toyCompiler Threads)
target_link_libraries(toyCompiler ${llvm_libs})
