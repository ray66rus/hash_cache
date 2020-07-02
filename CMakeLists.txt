cmake_minimum_required(VERSION 3.8)

project(HashCache VERSION 1.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(staticLib "Use static version of hash_cache library" ON)

add_executable(HashCache hashcache.cpp)

add_subdirectory(lib)

configure_file(hashcache.h.in hashcache.h)

target_include_directories(HashCache PRIVATE ${PROJECT_BINARY_DIR})

if(staticLib)
	target_link_libraries(HashCache hash_snapshot_static)
else()
	target_link_libraries(HashCache hash_snapshot)
endif()
