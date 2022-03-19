cmake_minimum_required(VERSION 3.7)
project(toyCompiler)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    codegen.cpp
    codegen.h
    main.cpp
    node.h
    ObjGen.cpp
    ObjGen.h
    parser.cpp
    parser.y
    tokens.cpp
    tokens.l
)

add_executable(toyCompiler ${SOURCE_FILES})