cmake_minimum_required(VERSION 3.20)

project(cnerium_http
    VERSION 0.1.0
    DESCRIPTION "Cnerium HTTP - HTTP primitives for the Cnerium web framework"
    LANGUAGES CXX
)

include(GNUInstallDirs)

# ========================
# Options
# ========================
option(CNERIUM_HTTP_BUILD_TESTS "Build http tests" ON)
option(CNERIUM_HTTP_BUILD_EXAMPLES "Build http examples" ON)

# ========================
# Global settings
# ========================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ========================
# Dependencies
# ========================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.vix/vix_deps.cmake")
    include("${CMAKE_CURRENT_SOURCE_DIR}/.vix/vix_deps.cmake")
endif()

find_package(cnerium_json CONFIG QUIET)
find_package(json CONFIG QUIET)

set(CNERIUM_HTTP_JSON_TARGET "")

if(TARGET json::json)
    set(CNERIUM_HTTP_JSON_TARGET json::json)
elseif(TARGET json)
    set(CNERIUM_HTTP_JSON_TARGET json)
elseif(TARGET cnerium::json)
    set(CNERIUM_HTTP_JSON_TARGET cnerium::json)
elseif(TARGET cnerium_json)
    set(CNERIUM_HTTP_JSON_TARGET cnerium_json)
endif()

if(CNERIUM_HTTP_JSON_TARGET STREQUAL "")
    message(FATAL_ERROR
        "cnerium json target not found.\n"
        "Tried: json::json, json, cnerium::json, cnerium_json\n"
        "Install dependencies with Vix:\n"
        "  vix add @cnerium/json\n"
        "  vix install"
    )
endif()

# ========================
# Library (header-only)
# ========================
add_library(http INTERFACE)
add_library(http::http ALIAS http)

target_compile_features(http INTERFACE cxx_std_20)

target_link_libraries(http
    INTERFACE
        ${CNERIUM_HTTP_JSON_TARGET}
)

target_include_directories(http INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# ========================
# Compiler options
# ========================
if(MSVC)
    target_compile_options(http INTERFACE /W4 /permissive-)
else()
    target_compile_options(http INTERFACE -Wall -Wextra -Wpedantic)
endif()

# ========================
# Target properties
# ========================
set_target_properties(http PROPERTIES
    EXPORT_NAME http
)

# ========================
# Examples
# ========================
if(CNERIUM_HTTP_BUILD_EXAMPLES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt")
    add_subdirectory(examples)
endif()

# ========================
# Tests
# ========================
if(CNERIUM_HTTP_BUILD_TESTS)
    include(CTest)
    enable_testing()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_basic.cpp")
        add_executable(http_basic_test tests/test_basic.cpp)
        target_link_libraries(http_basic_test PRIVATE http::http)
        add_test(NAME http.basic COMMAND http_basic_test)
    endif()
endif()

# ========================
# Summary
# ========================
message(STATUS "------------------------------------------------------")
message(STATUS "cnerium http configured (${PROJECT_VERSION})")
message(STATUS "JSON target:     ${CNERIUM_HTTP_JSON_TARGET}")
message(STATUS "Include dir:     ${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "Build tests:     ${CNERIUM_HTTP_BUILD_TESTS}")
message(STATUS "Build examples:  ${CNERIUM_HTTP_BUILD_EXAMPLES}")
message(STATUS "Build type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")
