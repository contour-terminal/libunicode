include(CPM)

CPMAddPackage(
  NAME fmt
  VERSION 20e4ef8b4c7ce67e880f28084af02fb9d1ce99c2
  URL https://github.com/fmtlib/fmt/archive/20e4ef8b4c7ce67e880f28084af02fb9d1ce99c2.zip
  URL_HASH SHA256=cc3e6064ebd56d432ba5afd74dc038dbb814e3740be476d5ca1620b6872e206a
)

if(LIBUNICODE_TESTING)
  CPMAddPackage(
    NAME catch2
    VERSION 6f21a3609cea360846a0ca93be55877cca14c86d
    URL https://github.com/catchorg/Catch2/archive/6f21a3609cea360846a0ca93be55877cca14c86d.zip
    URL_HASH SHA256=8aec86bc5ccfd963caf3dc4cd137001a7f80bf54d44c69b187cc92b40851399a
  )
endif()

