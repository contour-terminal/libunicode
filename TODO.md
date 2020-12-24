
# TODO

- [ ] rewrite test functions from `bool foo(x) ...` to `bool is_foo(x) ...`
- [ ] all about emoji flag sequences
- [ ] `bool is_mirrorred(char32_t) noexcept` (such as parenthesis, curly braces, brackets, ...)
  - also ability to get the mirrorring codepoint
- [ ] map codepoint to block (enum) - see Blocks.txt
- [ ] map coepoint to plane (enum)
- [ ] map block to codepoint range
- [ ] map plane to codepoint range
- [ ] provide C API binding for basic functionality
- [ ] `script_segmenter`: add support for commonPreferredScript tracking wrt brackets () [] {}.
- [ ] `script_segmenter`: test "foo(λ);" -> {Latin, Greek, Latin}
- [ ] `orientation_segmenter` (and integrate it into `run_segmenter` as well as its tests)
- [ ] mktables: `fmtlib` integration into `ucd_fmt.h` (without actually depending on fmtlib itself)
- [ ] mktables: `to_string` builder
- [ ] mktables: `to_type` builder
- [ ] mktables: pylint into CI
- [ ] clang-tidy into CI
- [ ] META: cmake install target (header files and .a file, executable)
- [ ] META: pkg-config file
- [ ] word segmentation (UTS algorithm)
- [ ] generic text segmentation (top level segmentation API suitable for text shaping implementations)
- [ ] CLI tool: unicode-inspect for inspecting input files by code point, grapheme cluster, word, script, ...
- [x] unit tests for most parts (wcwidth / segmentation)
- [x] README: list all TRs that are being implemented
- [x] API for accessing UCD properties
- [x] UTF8 <-> UTF32 conversion
- [x] grapheme segmentation (UTS algorithm)
- [x] symbol/emoji segmentation (UTS algorithm)
- [x] wcwidth equivalent (`unicode::width(char32_t)`)
- [x] script segmentation
- [x] `out<T>` helper to force explicit `ref(val)` for more readability.
- [x] `operator<<(ostream&, T)` for all UCD properties - in its own header file (`ucd_ostream.h`)
- [x] `emoji_segmenter`: test "x 😀 y" -> {Text, Emoji, Text}
- [x] make `run_segmenter` more templated / customizable
- [x] mktables: `enum class` builder

## Integration TODO

* [x] integrate into contour
* [ ] see if this makes sense: make use of this library in klex lexical scanner, to allow unicode input

