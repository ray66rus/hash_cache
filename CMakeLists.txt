cmake_minimum_required(VERSION 3.8)

project(HashCache VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(HashCache hashcache.cpp)

add_subdirectory(lib)

configure_file(hashcache.h.in hashcache.h)

target_include_directories(HashCache PRIVATE ${PROJECT_BINARY_DIR})

target_link_libraries(HashCache hash_snapshot)
