include(CPM)

set(3rdparty_catch2_version "6f21a3609cea360846a0ca93be55877cca14c86d" CACHE STRING "catch2: commit hash")
set(3rdparty_fmt_version "561834650aa77ba37b15f7e5c9d5726be5127df9" CACHE STRING "fmt: commit hash")

CPMAddPackage(
  NAME fmt
  VERSION ${3rdparty_fmt_version}
  URL https://github.com/fmtlib/fmt/archive/${3rdparty_fmt_version}.zip
  URL_HASH SHA256=310ba642b8944ecfc798fea39bfe66b91fd3c649d29c4fdfc218b0b2bb6c23d7
)

if(LIBUNICODE_TESTING)
  CPMAddPackage(
    NAME catch2
    VERSION ${3rdparty_catch2_version}
    URL https://github.com/catchorg/Catch2/archive/${3rdparty_catch2_version}.zip
    URL_HASH SHA256=8aec86bc5ccfd963caf3dc4cd137001a7f80bf54d44c69b187cc92b40851399a
  )
endif()

