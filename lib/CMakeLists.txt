cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(hash_snapshot SHARED
        src/snapshot.cpp
        "include/snapshot.hpp"
        )

target_include_directories(hash_snapshot
        PUBLIC "include"
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR} 
        )

set_target_properties(hash_snapshot PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        )

include(GenerateExportHeader)
generate_export_header(hash_snapshot)

target_include_directories(hash_snapshot
        INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
        )