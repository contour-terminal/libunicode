add_executable(uc-inspect uc-inspect.cpp)
target_link_libraries(uc-inspect fmt::fmt-header-only unicode)
install(TARGETS uc-inspect DESTINATION bin)
