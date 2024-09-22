cmake_minimum_required(VERSION 3.5.0)
project(kalastan VERSION 0.1.0 LANGUAGES C CXX)


set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(kalastan src/main.cpp src/parser/ks_parser.cpp)

