# This directory structure is being created by `scripts/install-deps.sh`
# and is used to inject all the dependencies the operating system's
# package manager did not provide (not found or too old version).

if(EXISTS ${PROJECT_SOURCE_DIR}/_deps/sources/CMakeLists.txt)
    message(STATUS "Embedding 3rdparty libraries ...")
    add_subdirectory(${PROJECT_SOURCE_DIR}/_deps/sources)
endif()

set(LIST ThirdParties)
macro(Thirdparty_Include_If_MIssing _TARGET _PACKAGE_NAME)
    if(${_PACKAGE_NAME} STREQUAL "")
        set(${_PACKAGE_NAME} ${_TARGET})
    endif()
    if (NOT TARGET ${_TARGET})
        find_package(${_PACKAGE_NAME} REQUIRED)
        list(APPEND ThirdParties ${_TARGET}_SYSDEP)
        set(THIRDPARTY_BUILTIN_${_TARGET} "system package")
    else()
        list(APPEND ThirdParties ${_TARGET}_EMBED)
        set(THIRDPARTY_BUILTIN_${_TARGET} "embedded")
    endif()
endmacro()

# TODO make me working
macro(ThirdPartiesSummary)
    message(STATUS "==============================================================================")
    message(STATUS "    ThirdParties")
    message(STATUS "------------------------------------------------------------------------------")
    foreach(TP ${ThirdParties})
        message(STATUS "${TP}\t\t${THIRDPARTY_BUILTIN_${TP}}")
    endforeach()
endmacro()

# Now, conditionally find all dependencies that were not included above
# via find_package, usually system installed packages.

if(LIBUNICODE_TESTING)
    if(TARGET Catch2::Catch2WithMain)
        set(THIRDPARTY_BUILTIN_Catch2 "embedded")
    else()
        find_package(Catch2 REQUIRED)
        set(THIRDPARTY_BUILTIN_Catch2 "system package")
    endif()
endif()

if(LIBUNICODE_BENCHMARK)
    if(TARGET benchmark::benchmark_main)
        set(THIRDPARTY_BUILTIN_benchmark "embedded")
    else()
        find_package(benchmark REQUIRED)
        set(THIRDPARTY_BUILTIN_benchmark "system package")
    endif()
endif()


macro(ThirdPartiesSummary2)
    message(STATUS "==============================================================================")
    message(STATUS "    ThirdParties")
    message(STATUS "------------------------------------------------------------------------------")
if(LIBUNICODE_TESTING)
    message(STATUS "Catch2              ${THIRDPARTY_BUILTIN_Catch2}")
endif()
if(LIBUNICODE_BENCHMARK)
    message(STATUS "Benchmark           ${THIRDPARTY_BUILTIN_benchmark}")
endif()
    message(STATUS "------------------------------------------------------------------------------")
endmacro()
