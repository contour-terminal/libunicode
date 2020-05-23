add_library(unicode STATIC
	utf8.h
	width.h width.cpp
    emoji_segmenter.h emoji_segmenter.cpp
    grapheme_segmenter.h
    ucd.h ucd.cpp
    script_segmenter.h script_segmenter.cpp
    text_segmenter.h text_segmenter.cpp
    word_segmenter.h
)
add_library(unicode::core ALIAS unicode)

target_include_directories(unicode PUBLIC ${PROJECT_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(unicode PUBLIC fmt::fmt-header-only)

# --------------------------------------------------------------------------------------------------------
# unicode_test

if(LIBUNICODE_TESTING)
    enable_testing()
    add_executable(unicode_test
		utf8_test.cpp
        emoji_segmenter_test.cpp
        grapheme_segmenter_test.cpp
        test_main.cpp
        unicode_test.cpp
        width_test.cpp
        script_segmenter_test.cpp
        text_segmenter_test.cpp
        word_segmenter_test.cpp
    )
    target_link_libraries(unicode_test fmt::fmt-header-only Catch2::Catch2 unicode)
    add_test(unicode_test ./unicode_test)
endif()
message(STATUS "[unicode] Compile unit tests: ${LIBUNICODE_TESTING}")


