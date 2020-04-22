cmake_minimum_required(VERSION 3.8)

project(HashCache VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(hashcache hashcache.cpp)

add_subdirectory(lib)

find_package(Boost 1.70.0
        REQUIRED
        COMPONENTS system
        )

target_link_libraries(hashcache hash_snapshot Boost::system)
