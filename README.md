[![C++17](https://img.shields.io/badge/standard-C%2B%2B%2017-blue.svg?logo=C%2B%2B)](https://isocpp.org/)
[![Build](https://github.com/christianparpart/contour/workflows/Ubuntu/badge.svg)](https://github.com/christianparpart/libunicode/actions?query=workflow%3AUbuntu)
[![Build](https://github.com/christianparpart/contour/workflows/Windows/badge.svg)](https://github.com/christianparpart/libunicode/actions?query=workflow%3AWindows)
[![Build](https://github.com/christianparpart/contour/workflows/MacOS/badge.svg)](https://github.com/christianparpart/libunicode/actions?query=workflow%3AMacOS)

# Modern C++17 Unicode Library

The goal of this library is to bring painless unicode support to C++ with simple and easy to understand APIs.

The API naming conventions are chosen to look familiar to those using the C++ standard libary.

### Feature Overview

- [ ] API for accessing UCD properties
- [x] UTF8 <-> UTF32 conversion
- [x] grapheme segmentation (UTS algorithm)
- [ ] word segmentation (UTS algorithm)
- [ ] script segmentation
- [x] symbol/emoji segmentation (UTS algorithm)
- [ ] generic text segmentation (top level segmentation API suitable for text shaping implementations)
- [ ] wcwidth equivalent (`unicode::width(char32_t)`)
- [ ] CLI tool: unicode-inspect for inspecting input files by code point, grapheme cluster, word, script, ...
- [ ] unit tests for most parts (wcwidth / segmentation)

### Contributing

- For filing issues please visit: https://github.com/christianparpart/libunicode/issues

### Users of this library

* [Contour Terminal Emulator](https://github.com/christianparpart/contour/)

### License

not decided yet. Apache or MIT?
