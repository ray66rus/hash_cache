cmake_minimum_required(VERSION 3.4)

project(HashCache VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(hashcache hashcache.cpp)

target_link_libraries(hashcache PUBLIC hash_snapshot)

add_subdirectory(lib)
list(APPEND EXTRA_LIBS hash_snapshot)

target_include_directories(hashcache PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          )