cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(libunicode VERSION "0.0.1" LANGUAGES CXX)

set(MASTER_PROJECT OFF)
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(MASTER_PROJECT ON)
endif()

# setting defaults
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    add_compile_options(-Wall)
    add_compile_options(-Wextra)
    if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
        add_definitions(-D_GLIBCXX_DEBUG)
    endif()
elseif(DEFINED MSVC)
    add_definitions(-DNOMINMAX)
    add_compile_options(/utf-8)
endif()

include(EnableCcache)
include(ClangTidy)
include(PedanticCompiler)

set(CMAKE_EXPORT_COMPILE_COMMANDS ${MASTER_PROJECT})
option(LIBUNICODE_COVERAGE "libunicode: Builds with codecov [default: OFF]" OFF)
option(LIBUNICODE_EXAMPLES "libunicode: Enables building of example programs. [default: ${MASTER_PROJECT}]" ${MASTER_PROJECT})
option(LIBUNICODE_TESTING "libunicode: Enables building of unittests for libunicode [default: ${MASTER_PROJECT}" ${MASTER_PROJECT})
option(LIBUNICODE_TOOLS "libunicode: Builds CLI tools [default: ${MASTER_PROJECT}]" ${MASTER_PROJECT})
option(LIBUNICODE_UCD_PATH "libunicode: Path to UCD directory for rebuilding UCD API & tables (an extracted UCD.zip)." "")

# ----------------------------------------------------------------------------
# code coverage

if(LIBUNICODE_COVERAGE AND NOT MSVC)
    add_compile_options(-g --coverage)
    set(CMAKE_EXE_LINKER_FLAGS "--coverage ${CMAKE_EXE_LINKER_FLAGS}")
    message("-- [code coverage] Enabled.")
else()
    message("-- [code coverage] Disabled.")
endif()

# ----------------------------------------------------------------------------
# 3rdparty dependencies

if(NOT TARGET fmt)
    find_package(fmt REQUIRED)
endif()

if(LIBUNICODE_TESTING)
    find_package(Catch2 REQUIRED)
endif()

# ----------------------------------------------------------------------------

add_subdirectory(src/unicode)
add_subdirectory(src/tools)

if("${CCACHE}" STREQUAL "")
    set(USING_CCACHE_STRING "OFF")
else()
    set(USING_CCACHE_STRING "${CCACHE}")
endif()

message(STATUS "------------------------------------------------------------------------------")
message(STATUS "    libunicode (version ${libunicode_VERSION}${libunicode_VERSION_SUFFIX})")
message(STATUS "------------------------------------------------------------------------------")
message(STATUS "Build type:                  ${CMAKE_BUILD_TYPE}")
message(STATUS "Build unit tests:            ${LIBUNICODE_TESTING}")
message(STATUS "Build tools:                 ${LIBUNICODE_TOOLS}")
message(STATUS "Using ccache:                ${USING_CCACHE_STRING}")
message(STATUS "Using UCD source:            ${LIBUNICODE_UCD_PATH}")
message(STATUS "Enable clang-tidy:           ${ENABLE_TIDY} (${CMAKE_CXX_CLANG_TIDY})")
message(STATUS "------------------------------------------------------------------------------")
