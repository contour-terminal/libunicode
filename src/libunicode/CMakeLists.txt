include(GNUInstallDirs)

function(ExtractZipArchive ZIP_FILE OUTPUT_DIR)
    if(CMAKE_VERSION VERSION_LESS 3.18)
        # Use the older method for versions prior to CMake 3.18
        execute_process(COMMAND unzip -o "${ZIP_FILE}" -d "${OUTPUT_DIR}")
    else()
        # Use the newer method introduced in CMake 3.18 and later
        file(ARCHIVE_EXTRACT INPUT ${ZIP_FILE} DESTINATION ${OUTPUT_DIR})
    endif()
endfunction()

# Check whether all auto-generated source files already exist.
# When they do, skip UCD download and the code-generation pipeline.
set(_LIBUNICODE_GENERATED_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/ucd.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ucd.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ucd_enums.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ucd_fmt.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ucd_ostream.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/case_normalization_data.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/codepoint_properties_data.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/codepoint_properties_data.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/codepoint_properties_names.cpp"
)

set(LIBUNICODE_HAS_PREGENERATED_FILES TRUE)
foreach(_gen_file IN LISTS _LIBUNICODE_GENERATED_FILES)
    if(NOT EXISTS "${_gen_file}")
        set(LIBUNICODE_HAS_PREGENERATED_FILES FALSE)
        break()
    endif()
endforeach()

# Automatically fetch Unicode database if not present.
# Skip download entirely when all generated files already exist on disk,
# since neither mktables.py nor unicode_tablegen will need to run.
if(LIBUNICODE_HAS_PREGENERATED_FILES)
    message(STATUS "[libunicode] All generated source files found; skipping UCD download and code generation.")
else()
    if(NOT IS_DIRECTORY "${LIBUNICODE_UCD_DIR}")
        if(NOT EXISTS "${LIBUNICODE_UCD_ZIP_FILE}")
            file(DOWNLOAD ${LIBUNICODE_UCD_ZIP_DOWNLOAD_URL} ${LIBUNICODE_UCD_ZIP_FILE} SHOW_PROGRESS STATUS LIBUNICODE_UCD_ZIP_DOWNLOAD_STATUS EXPECTED_MD5 ${LIBUNICODE_UCD_MD5})
        endif()
        ExtractZipArchive("${LIBUNICODE_UCD_ZIP_FILE}" "${LIBUNICODE_UCD_DIR}")
    endif()
endif()

# =========================================================================================================

set(LIBUNICODE_LIB_MODE)
if(LIBUNICODE_BUILD_STATIC)
    set(LIBUNICODE_LIB_MODE "STATIC")
else()
    set(LIBUNICODE_LIB_MODE "SHARED")
endif()
message(STATUS "libunicode library build mode: ${LIBUNICODE_LIB_MODE}")

add_library(unicode_ucd ${LIBUNICODE_LIB_MODE}
    ucd.cpp
    ucd.h
    ucd_enums.h
    ucd_fmt.h
    ucd_ostream.h
)
add_library(unicode::ucd ALIAS unicode_ucd)
set_target_properties(unicode_ucd PROPERTIES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)
target_include_directories(unicode_ucd PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
                                              $<INSTALL_INTERFACE:include>)

# =========================================================================================================

# The code-generation pipeline (tablegen, loader) is needed only when:
# 1. Not building for Emscripten (which can't run tablegen), AND
# 2. Pre-generated files are not already present on disk.
set(LIBUNICODE_NEEDS_TABLEGEN FALSE)
if(NOT EMSCRIPTEN AND NOT LIBUNICODE_HAS_PREGENERATED_FILES)
    set(LIBUNICODE_NEEDS_TABLEGEN TRUE)
endif()

if(LIBUNICODE_NEEDS_TABLEGEN)
    # Unified C++ tablegen: self-contained, no library dependencies.
    # Generates all 9 UCD source files in a single invocation.
    add_executable(unicode_tablegen
        tablegen/tablegen_main.cpp
        tablegen/ucd_parser.cpp
        tablegen/enum_generator.cpp
        tablegen/ucd_api_generator.cpp
        tablegen/case_norm_generator.cpp
        tablegen/multistage_generator.cpp
    )
    set_target_properties(unicode_tablegen PROPERTIES CMAKE_BUILD_TYPE Release)
    target_compile_definitions(unicode_tablegen PRIVATE LIBUNICODE_UCD_VERSION="${LIBUNICODE_UCD_VERSION}")
    target_include_directories(unicode_tablegen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

    add_custom_command(
        OUTPUT ${_LIBUNICODE_GENERATED_FILES}
        COMMAND unicode_tablegen
            "${LIBUNICODE_UCD_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "unicode::precompiled"
        DEPENDS unicode_tablegen
        COMMENT "Generating all UCD tables from ${LIBUNICODE_UCD_DIR}"
        VERBATIM
        USES_TERMINAL
    )
endif()

# =========================================================================================================


if(LIBUNICODE_SIMD_IMPLEMENTATION STREQUAL "std" OR LIBUNICODE_SIMD_IMPLEMENTATION STREQUAL "intrinsics")
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
        set(LIBUNICODE_SIMD_SOURCES
            simd_detector.cpp
            scan256.cpp
            scan512.cpp
            )
        set_source_files_properties(
            scan256.cpp
            PROPERTIES
            COMPILE_FLAGS
            -mavx2
            )
        set_source_files_properties(
            scan512.cpp
            PROPERTIES
            COMPILE_FLAGS
            "-mavx512f -mavx512bw"
            )
    endif()
endif()

add_library(unicode ${LIBUNICODE_LIB_MODE}
    capi.cpp
    case_mapping.cpp
    codepoint_properties.cpp
    emoji_segmenter.cpp
    grapheme_segmenter.cpp
    normalization.cpp
    scan.cpp
    script_segmenter.cpp
    utf8.cpp
    width.cpp
    ${LIBUNICODE_SIMD_SOURCES}

    # auto-generated by unicode_tablegen
    codepoint_properties_data.h
    codepoint_properties_data.cpp
    codepoint_properties_names.cpp

    # auto-generated by mkcasetables.py
    case_normalization_data.h
)

if(LIBUNICODE_SIMD_IMPLEMENTATION STREQUAL "std")
    target_compile_definitions(unicode PRIVATE LIBUNICODE_USE_STD_SIMD)
elseif(LIBUNICODE_SIMD_IMPLEMENTATION STREQUAL "intrinsics")
    target_compile_definitions(unicode PRIVATE LIBUNICODE_USE_INTRINSICS)
endif()

set(public_headers
    capi.h
    case_mapping.h
    codepoint_properties.h
    convert.h
    emoji_segmenter.h
    grapheme_segmenter.h
    intrinsics.h
    multistage_table_view.h
    normalization.h
    run_segmenter.h
    scan.h
    script_segmenter.h
    support.h
    utf8.h
    utf8_grapheme_segmenter.h
    width.h
    word_segmenter.h
)

set(private_headers
    multistage_table_generator.h
    scoped_timer.h
)

set_target_properties(unicode PROPERTIES PUBLIC_HEADER "${public_headers}")
set_target_properties(unicode PROPERTIES PRIVATE_HEADER "${private_headers}")
set_target_properties(unicode PROPERTIES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)

add_library(unicode::unicode ALIAS unicode)
add_library(unicode::core ALIAS unicode)
target_include_directories(unicode PUBLIC $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/src>
                                          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(unicode PUBLIC unicode::ucd)


# {{{ clean-generated target
add_custom_target(clean-generated
    COMMAND ${CMAKE_COMMAND} -E rm -f
        "${CMAKE_CURRENT_SOURCE_DIR}/ucd.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ucd.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/ucd_enums.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/ucd_fmt.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/ucd_ostream.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/case_normalization_data.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/codepoint_properties_data.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/codepoint_properties_data.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/codepoint_properties_names.cpp"
    COMMENT "Removing all auto-generated UCD source files. Reconfigure to regenerate."
)
# }}}

# {{{ installation
set(LIBUNICODE_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/libunicode" CACHE PATH "Installation directory for cmake files, a relative path that will be joined with ${CMAKE_INSTALL_PREFIX} or an absolute path.")
set(LIBUNICODE_INSTALL_CMAKE_FILES ${MASTER_PROJECT} CACHE BOOL "Decides whether or not to install CMake config and -version files.")

# Create and install package configuration and version files.
# Install library and headers.
set(_LIBUNICODE_INSTALL_TARGETS unicode_ucd unicode)
install(TARGETS ${_LIBUNICODE_INSTALL_TARGETS}
        EXPORT libunicode-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libunicode"
        PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libunicode"
        FRAMEWORK DESTINATION "."
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(
    FILES
        ucd.h
        ucd_enums.h
        ucd_fmt.h
        ucd_ostream.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libunicode"
)
# }}}

# {{{ Generate the version, config and target files
if(LIBUNICODE_INSTALL_CMAKE_FILES)
    # Install version, config and target files.
    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/libunicode-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_package_config_file(libunicode-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/libunicode-config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libunicode
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libunicode-config.cmake
                  ${CMAKE_CURRENT_BINARY_DIR}/libunicode-config-version.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libunicode
    )

    install(EXPORT libunicode-targets
            DESTINATION ${LIBUNICODE_CMAKE_DIR}
            NAMESPACE unicode::
    )
endif()
# }}}

# {{{ unicode_test
if(LIBUNICODE_TESTING)
    add_executable(unicode_test
        bidi_mirroring_test.cpp
        capi_test.cpp
        case_mapping_test.cpp
        convert_test.cpp
        emoji_segmenter_test.cpp
        grapheme_segmenter_test.cpp
        normalization_test.cpp
        run_segmenter_test.cpp
        scan_test.cpp
        script_segmenter_test.cpp
        test_main.cpp
        unicode_test.cpp
        utf8_grapheme_segmenter_test.cpp
        utf8_test.cpp
        width_test.cpp
        word_segmenter_test.cpp
    )

    if(NOT Catch2_FOUND)
        # Suppress all warnings for embedded third-party Catch2.
        # https://github.com/catchorg/Catch2/issues/2583
        suppress_thirdparty_warnings(Catch2)
        suppress_thirdparty_warnings(Catch2WithMain)
        get_target_property(CATCH2_INCLUDE_DIRS Catch2 INTERFACE_INCLUDE_DIRECTORIES)
        target_include_directories(Catch2 SYSTEM INTERFACE ${CATCH2_INCLUDE_DIRS})
    endif()

    target_link_libraries(unicode_test unicode Catch2::Catch2WithMain)
    add_test(unicode_test unicode_test)
endif()
# }}}



# {{{ unicode_benchmark
if(LIBUNICODE_BENCHMARK)
    if(NOT benchmark_FOUND)
        # Suppress all warnings for embedded third-party benchmark.
        suppress_thirdparty_warnings(benchmark)
        get_target_property(BENCHMARK_INCLUDE_DIRS benchmark INTERFACE_INCLUDE_DIRECTORIES)
        target_include_directories(benchmark SYSTEM INTERFACE ${BENCHMARK_INCLUDE_DIRS})
    endif()

    add_executable(libunicode_benchmark
        benchmark.cpp
    )
    target_compile_features(libunicode_benchmark PRIVATE cxx_std_20)
    target_link_libraries(libunicode_benchmark PRIVATE benchmark::benchmark unicode)
endif()
# }}}
