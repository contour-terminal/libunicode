[![C++17](https://img.shields.io/badge/standard-C%2B%2B%2017-blue.svg?logo=C%2B%2B)](https://isocpp.org/)
[![Build](https://github.com/christianparpart/contour/workflows/Ubuntu/badge.svg)](https://github.com/christianparpart/libunicode/actions?query=workflow%3AUbuntu)
[![Windows](https://github.com/christianparpart/libunicode/workflows/Windows/badge.svg)](https://github.com/christianparpart/libunicode/actions?query=workflow%3AWindows)
[![OS/X](https://github.com/christianparpart/libunicode/workflows/MacOS/badge.svg)](https://github.com/christianparpart/libunicode/actions?query=workflow%3AMacOS)

# Modern C++17 Unicode Library

The goal of this library is to bring painless unicode support to C++ with simple and easy to understand APIs.

The API naming conventions are chosen to look familiar to those using the C++ standard libary.

### Feature Overview

- [x] API for accessing UCD properties
- [x] UTF8 <-> UTF32 conversion
- [x] grapheme segmentation (UTS algorithm)
- [ ] word segmentation (UTS algorithm)
- [ ] script segmentation
- [x] symbol/emoji segmentation (UTS algorithm)
- [ ] generic text segmentation (top level segmentation API suitable for text shaping implementations)
- [ ] wcwidth equivalent (`unicode::width(char32_t)`)
- [ ] CLI tool: unicode-inspect for inspecting input files by code point, grapheme cluster, word, script, ...
- [ ] unit tests for most parts (wcwidth / segmentation)

### Integrate with your CMake project

```sh
git submodule add --name libunicode https://github.com/christianparpart/libunicode 3rdparty/libunicode
```

```cmake
add_subdirectory(3rdparty/libunicode)

add_executable(your_tool your_tool.cpp)
target_link_libraries(your_tool PUBLIC unicode::core)
```

### Contributing

- for filing issues please visit: https://github.com/christianparpart/libunicode/issues
- fork and create pull requests:  https://github.com/christianparpart/libunicode/pulls
- I am also happy to just receive code reviews, of
- your can help with documentation, or
- general feedback is also very welcome

### Users of this library

* [Contour Terminal Emulator](https://github.com/christianparpart/contour/)

### License

not decided yet. Apache or MIT?
