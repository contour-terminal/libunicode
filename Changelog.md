## 0.2.0 (2022-11-13)

- Slightly improve performance of grapheme cluster segmentation.
- Fixes grapheme cluster segmentation of multiple consecutive regional flags.
- Add access to Age property of a codepoint (giving information about at which Unicode version a codepoint was introduced).
- Add access to the assigned name of a codepoint.
- unicode-query: Now also prints name and age properties.
- CMake install target also installs header and library (not just tools).
- Reduce number of dependencies down to fmtlib and (for unit tests) Catch2.
- Enables libunicode to be found via CMake's `find_package()`.
- Improved default installation directories on UNIX via GNUInstallDirs helper.

## 0.1.0 (2022-11-03)

While version 0.1.0 sounds like a small number, this project is out there since quite some years already
and actively used by Contour Terminal.

The biggest movements lately are major performance improvements in accessing Unicode properties,
fixing bugs as usual, and apart from being a modern C++ Unicode library, we've now also added
a command line tool to query Unicode properties in the hope it'll be useful to you.

