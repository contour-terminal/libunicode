if(LIBUNICODE_TOOLS)
    add_executable(unicode-query unicode-query.cpp)
    target_link_libraries(unicode-query unicode)
    target_compile_definitions(unicode-query PRIVATE
        LIBUNICODE_VERSION="${PROJECT_VERSION}"
        LIBUNICODE_UCD_VERSION="${LIBUNICODE_UCD_VERSION}"
    )
    if(LIBUNICODE_BUILD_STATIC AND UNIX AND NOT APPLE)
        target_link_libraries(unicode-query "-static")
    endif()
    install(TARGETS unicode-query DESTINATION bin)
endif()
