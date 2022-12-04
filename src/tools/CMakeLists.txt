if(LIBUNICODE_TOOLS)
    # This tool is going to be replaced by unicode-query,
    # So disable it early to not be included in the release.
    #
    # add_executable(uc-inspect uc-inspect.cpp)
    # target_link_libraries(uc-inspect fmt::fmt-header-only unicode)
    # install(TARGETS uc-inspect DESTINATION bin)

    add_executable(unicode-query unicode-query.cpp)
    target_link_libraries(unicode-query unicode)
    if(UNIX)
        target_link_libraries(unicode-query "-static")
    endif()
    install(TARGETS unicode-query DESTINATION bin)
endif()
