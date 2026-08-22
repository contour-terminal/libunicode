#!/bin/sh
# gcov-tool shim for `lcov`/`geninfo` when the coverage build used Clang.
#
# lcov probes its configured gcov tool with a bare "--version" and expects
# real gcov's semantics (a one-line version banner containing "LLVM" for the
# Clang case, which geninfo already knows how to map to a gcov version).
# LLVM's own `gcov` emulation mode does not implement --version as a
# standalone query -- it tries to treat it as a source-file argument
# instead -- so intercept that probe here and answer it directly; every
# other invocation is passed through to `llvm-cov gcov` unchanged.
set -eu

# Resolve the llvm-cov that matches the compiler actually used, rather than
# guessing an unversioned "llvm-cov" that may not exist (Ubuntu ships only
# versioned binaries, e.g. llvm-cov-18). Override with $LLVM_COV if needed.
CXX="${CXX:-clang++}"
LLVM_COV="${LLVM_COV:-$("$CXX" --print-prog-name=llvm-cov)}"

if [ "${1:-}" = "--version" ]; then
    exec "$LLVM_COV" --version
fi
exec "$LLVM_COV" gcov "$@"
