cmake_minimum_required(VERSION 3.10)

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
elseif(DEFINED MSVC)
    add_definitions(-DNOMINMAX)
endif()

include(EnableCcache)
include(ClangTidy)

set(CMAKE_EXPORT_COMPILE_COMMANDS ${MASTER_PROJECT})
option(LIBUNICODE_TESTING "Enables building of unittests for libunicode [default: ${MASTER_PROJECT}" ${MASTER_PROJECT})
option(LIBUNICODE_EMBEDDED_FMTLIB "Uses embedded fmtlib [default: ${MASTER_PROJECT}" ${MASTER_PROJECT})
option(LIBUNICODE_EMBEDDED_CATCH2 "Uses embedded catch2 for testing [default: ${MASTER_PROJECT}]" ${MASTER_PROJECT})
option(LIBUNICODE_EXAMPLES "Enables building of example programs. [default: ${MASTER_PROJECT}]" ${MASTER_PROJECT})
option(LIBUNICODE_COVERAGE "Builds with codecov [default: OFF]" OFF)

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

if(LIBUNICODE_EMBEDDED_FMTLIB)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/fmt" EXCLUDE_FROM_ALL)
    add_definitions(-DFMT_USE_WINDOWS_H=0)
else()
    # master project must provide its own fmtlib
endif()

if(LIBUNICODE_TESTING AND LIBUNICODE_EMBEDDED_CATCH2)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/catch2")
else()
    # master project must provide its own fmtlib
endif()

add_subdirectory(src/unicode)
add_subdirectory(src/tools)
