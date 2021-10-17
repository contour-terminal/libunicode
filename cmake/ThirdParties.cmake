include(CPM)

set(3rdparty_catch2_version "6f21a3609cea360846a0ca93be55877cca14c86d" CACHE STRING "catch2: commit hash")
set(3rdparty_fmt_version "7a604cdd98eff2883b9dbd7d1ca1ddd8c147b80e" CACHE STRING "fmt: commit hash")

CPMAddPackage(
  NAME fmt
  VERSION ${3rdparty_fmt_version}
  URL https://github.com/fmtlib/fmt/archive/${3rdparty_fmt_version}.zip
  URL_HASH SHA256=3e2745554689c34fe23b6d5575c7ee0c74d2e2b83da15e2326263f6fec8ed409
)

if(LIBUNICODE_TESTING)
  CPMAddPackage(
    NAME catch2
    VERSION ${3rdparty_catch2_version}
    URL https://github.com/catchorg/Catch2/archive/${3rdparty_catch2_version}.zip
    URL_HASH SHA256=8aec86bc5ccfd963caf3dc4cd137001a7f80bf54d44c69b187cc92b40851399a
  )
endif()

