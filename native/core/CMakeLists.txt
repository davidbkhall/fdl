# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20)

include(cmake/FDLVersion.cmake)

project(fdl_core
  VERSION ${FDL_CORE_VERSION}
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific symbol visibility
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

# Export macros
include(GenerateExportHeader)

# jsoncons — PRIVATE dependency (not exposed in C ABI)
# Provides ordered JSON (ojson) and Draft 2020-12 schema validation
include(FetchContent)
FetchContent_Declare(
  jsoncons
  GIT_REPOSITORY https://github.com/danielaparker/jsoncons.git
  GIT_TAG v1.5.0
  GIT_SHALLOW TRUE
)
# Disable jsoncons' own tests/examples
set(JSONCONS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(jsoncons)

# Generate schema header from FDL schema files (embeds at compile time)
set(FDL_SCHEMA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../schema")
set(FDL_SCHEMA_OUT "${CMAKE_CURRENT_BINARY_DIR}/fdl_schema_data.h")
execute_process(
  COMMAND ${CMAKE_COMMAND}
    -DFDL_SCHEMA_DIR=${FDL_SCHEMA_DIR}
    -DFDL_SCHEMA_OUT=${FDL_SCHEMA_OUT}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateSchemaHeader.cmake
)

# Core shared library
add_library(fdl_core SHARED
  src/fdl_abi.cpp
  src/fdl_handles.cpp
  src/fdl_rounding.cpp
  src/fdl_value_types.cpp
  src/fdl_canonical.cpp
  src/fdl_doc.cpp
  src/fdl_doc_api.cpp
  src/fdl_validate.cpp
  src/fdl_validate_api.cpp
  src/fdl_geometry.cpp
  src/fdl_geometry_api.cpp
  src/fdl_pipeline.cpp
  src/fdl_pipeline_api.cpp
  src/fdl_collections_api.cpp
  src/fdl_accessors_api.cpp
  src/fdl_builder.cpp
  src/fdl_builder_api.cpp
  src/fdl_framing.cpp
  src/fdl_framing_api.cpp
  src/fdl_template.cpp
  src/fdl_template_api.cpp
)

target_include_directories(fdl_core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

generate_export_header(fdl_core
  EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/fdl_export.h
  EXPORT_MACRO_NAME FDL_API
)

target_link_libraries(fdl_core PRIVATE jsoncons)

target_compile_definitions(fdl_core PRIVATE fdl_core_EXPORTS)

# Pass ABI version to source
target_compile_definitions(fdl_core PRIVATE
  FDL_ABI_VERSION_MAJOR=${FDL_ABI_VERSION_MAJOR}
  FDL_ABI_VERSION_MINOR=${FDL_ABI_VERSION_MINOR}
  FDL_ABI_VERSION_PATCH=${FDL_ABI_VERSION_PATCH}
)

# macOS-specific: set install name for dylib
if(APPLE)
  set_target_properties(fdl_core PROPERTIES
    INSTALL_RPATH "@loader_path"
    BUILD_WITH_INSTALL_RPATH ON
  )
endif()

# Tests (optional, gated by BUILD_TESTING)
option(FDL_BUILD_TESTS "Build fdl_core tests" ON)
if(FDL_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
