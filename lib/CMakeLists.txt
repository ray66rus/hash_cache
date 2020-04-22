cmake_minimum_required(VERSION 3.8)

project(HashSnapshotLib VERSION 0.9.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(obj_hash_snapshot
        OBJECT
        src/snapshot.cpp
        src/storage.cpp
        src/file_info.cpp
        src/entry.cpp
        includes/hash_snapshot/hash_snapshot.hpp
        includes/hash_snapshot/storage.hpp
        includes/hash_snapshot/exception.hpp
        includes/hash_snapshot/file_info.hpp
        includes/hash_snapshot/entry.hpp
        )

set_property(TARGET obj_hash_snapshot PROPERTY POSITION_INDEPENDENT_CODE 1)

find_package(OpenSSL REQUIRED)

include(GenerateExportHeader)
generate_export_header(obj_hash_snapshot
        BASE_NAME hash_snapshot
        )

set(INCLUDES
        ${CMAKE_CURRENT_BINARY_DIR}
        includes
        ${OPENSSL_INCLUDE_DIR}
        )

include_directories(${INCLUDES})

set_target_properties(obj_hash_snapshot
        PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        C_VISIBILITY_PRESET hidden
        )

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
target_compile_definitions(obj_hash_snapshot PRIVATE DEBUG_ON)
endif()

# SHARED
add_library(hash_snapshot SHARED $<TARGET_OBJECTS:obj_hash_snapshot>)

target_include_directories(hash_snapshot
        INTERFACE ${INCLUDES}
        )

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
target_link_libraries(hash_snapshot
        PRIVATE
        "stdc++fs"
        "-Wl,--exclude-libs,ALL"
        )
endif()

target_link_libraries(hash_snapshot PRIVATE OpenSSL::Crypto)

# STATIC
add_library(hash_snapshot_static STATIC $<TARGET_OBJECTS:obj_hash_snapshot>)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
target_link_libraries(hash_snapshot_static PRIVATE "stdc++fs")
endif()

target_link_libraries(hash_snapshot_static PRIVATE OpenSSL::Crypto)

# Tests
enable_testing()
add_subdirectory(tests)