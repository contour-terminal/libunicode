#! /bin/bash

set -ex

BUILD_DIR=${BUILD_DIR:-build}
CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE:-RelWithDebInfo}"

# Populate CXX if COMPILER env var was passed.
if [ "$COMPILER" != "" ]; then
    VER=`echo $COMPILER | awk '{print $2}'`
    NAME=`echo $COMPILER | awk '{print tolower($1);}'`
    [[ "$NAME" = "clang" ]] && NAME="clang++"
    [[ "$NAME" = "gcc" ]] && NAME="g++"
    export CXX="$NAME-$VER"
fi

prepare_build_ubuntu()
{
   cmake \
      -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}" \
      -DPEDANTIC_COMPILER="OFF" \
      -DPEDANTIC_COMPILER_WERROR="OFF" \
      -DLIBUNICODE_BENCHMARK="ON" \
      -S . -B ${BUILD_DIR} \
      ${EXTRA_CMAKE_FLAGS}
}

main_linux()
{
    local ID=`lsb_release --id | awk '{print $NF}'`

    case "${ID}" in
        Ubuntu)
            prepare_build_ubuntu
            ;;
        *)
            echo "No automated build configuration is available yet."
            ;;
    esac
}

main_darwin()
{
    echo "No automated build configuration is available yet."
}

main()
{
    case "$OSTYPE" in
        linux-gnu*)
            main_linux
            ;;
        darwin*)
            main_darwin
            ;;
        *)
            echo "OS not supported."
            ;;
    esac
}

main
