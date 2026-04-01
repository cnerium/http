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

if(NOT TARGET cnerium::json AND NOT TARGET cnerium_json)
    find_package(cnerium_json CONFIG QUIET)
    find_package(json CONFIG QUIET)
endif()

set(CNERIUM_HTTP_JSON_TARGET "")
if(TARGET cnerium::json)
    set(CNERIUM_HTTP_JSON_TARGET cnerium::json)
elseif(TARGET cnerium_json)
    set(CNERIUM_HTTP_JSON_TARGET cnerium_json)
endif()

if(CNERIUM_HTTP_JSON_TARGET STREQUAL "")
    message(FATAL_ERROR
        "cnerium json target not found.\n"
        "Install dependencies with Vix:\n"
        "  vix add @cnerium/json\n"
        "  vix install"
    )
endif()

# ========================
# Library (header-only)
# ========================
add_library(cnerium_http INTERFACE)
add_library(cnerium::http ALIAS cnerium_http)

target_compile_features(cnerium_http INTERFACE cxx_std_20)

target_link_libraries(cnerium_http
    INTERFACE
        ${CNERIUM_HTTP_JSON_TARGET}
)

target_include_directories(cnerium_http
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# ========================
# Compiler options
# ========================
if(MSVC)
    target_compile_options(cnerium_http INTERFACE /W4 /permissive-)
else()
    target_compile_options(cnerium_http INTERFACE -Wall -Wextra -Wpedantic)
endif()

# ========================
# Target properties
# ========================
set_target_properties(cnerium_http PROPERTIES
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
        add_executable(cnerium_http_test_basic tests/test_basic.cpp)
        target_link_libraries(cnerium_http_test_basic PRIVATE cnerium::http)
        target_compile_features(cnerium_http_test_basic PRIVATE cxx_std_20)

        if(MSVC)
            target_compile_options(cnerium_http_test_basic PRIVATE /W4 /permissive-)
        else()
            target_compile_options(cnerium_http_test_basic PRIVATE -Wall -Wextra -Wpedantic)
        endif()

        add_test(NAME cnerium_http.basic COMMAND cnerium_http_test_basic)
    endif()
endif()

# ========================
# Summary
# ========================
message(STATUS "------------------------------------------------------")
message(STATUS "cnerium::http configured (${PROJECT_VERSION})")
message(STATUS "JSON target:     ${CNERIUM_HTTP_JSON_TARGET}")
message(STATUS "Include dir:     ${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "Build tests:     ${CNERIUM_HTTP_BUILD_TESTS}")
message(STATUS "Build examples:  ${CNERIUM_HTTP_BUILD_EXAMPLES}")
message(STATUS "Build type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")
