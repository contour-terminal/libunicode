include(CPM)

set(3rdparty_catch2_version "6f21a3609cea360846a0ca93be55877cca14c86d" CACHE STRING "catch2: commit hash")
set(3rdparty_fmt_version "8465869d7b04ae00f657332b3374ecf95b5e49c0" CACHE STRING "fmt: commit hash")

CPMAddPackage(
  NAME fmt
  VERSION ${3rdparty_fmt_version}
  URL https://github.com/fmtlib/fmt/archive/${3rdparty_fmt_version}.zip
  URL_HASH SHA256=3c0a45ee3e3688b407b4243e38768f346e75ec4a9b16cefbebf17252048395da
)

if(LIBUNICODE_TESTING)
  CPMAddPackage(
    NAME catch2
    VERSION ${3rdparty_catch2_version}
    URL https://github.com/catchorg/Catch2/archive/${3rdparty_catch2_version}.zip
    URL_HASH SHA256=8aec86bc5ccfd963caf3dc4cd137001a7f80bf54d44c69b187cc92b40851399a
  )
endif()

