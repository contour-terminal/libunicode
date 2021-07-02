include(CPM)

CPMAddPackage(
  NAME fmt
  URL https://github.com/fmtlib/fmt/archive/master.zip
  VERSION master
#  DOWNLOAD_ONLY YES
)

CPMAddPackage(
  NAME catch2
  URL https://github.com/catchorg/Catch2/archive/devel.zip
  VERSION devel
#  DOWNLOAD_ONLY YES
)


