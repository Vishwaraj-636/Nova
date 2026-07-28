cmake_minimum_required(VERSION 3.16)

project(Nova)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(
    nova
    src/main.cpp
    src/lexer.cpp
    src/interpreter.cpp
)