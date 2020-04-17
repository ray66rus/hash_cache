cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Catch2 REQUIRED CONFIG)

add_executable(tests
        _normal_snapshot_file.h
        _fixtures.hpp
        _fixtures.cpp
        test-main.cpp
        test-file_info.cpp
        test-entry.cpp
        test-storage.cpp
        test-snapshot.cpp
        )

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(tests
        Catch2::Catch2
        hash_snapshot_static
        )

include(CTest)
include(Catch)

catch_discover_tests(tests EXTRA_ARGS "-a")
