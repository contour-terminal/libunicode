include(CPM)

set(3rdparty_catch2_version "22750cde0e0d2c02ab447e2500422f2711356053" CACHE STRING "catch2: commit hash")
set(3rdparty_fmt_version "3b6e409cd8573f63e4acad7717d9082bd898ec87" CACHE STRING "fmt: commit hash")

CPMAddPackage(
  NAME fmt
  VERSION ${3rdparty_fmt_version}
  URL https://github.com/fmtlib/fmt/archive/${3rdparty_fmt_version}.zip
  URL_HASH SHA256=d5a6f6e3431fec6332021dc69a14dd11b85b0103b6317d33b2e5e5a91fd08add
)

if(LIBUNICODE_TESTING)
  CPMAddPackage(
    NAME catch2
    VERSION ${3rdparty_catch2_version}
    URL https://github.com/catchorg/Catch2/archive/${3rdparty_catch2_version}.zip
    URL_HASH SHA256=ab21f8c8804f3c9987d21a731440611d8dfecf8a80efcd2e46bf67a3d9bdf967
  )
endif()

