cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

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

include(FetchContent)
FetchContent_Declare(
	catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	)
FetchContent_MakeAvailable(catch2)

target_link_libraries(tests
        Catch2::Catch2
        hash_snapshot_static
        )

include(CTest)
include("${catch2_SOURCE_DIR}/contrib/Catch.cmake")

catch_discover_tests(tests EXTRA_ARGS "-a")
