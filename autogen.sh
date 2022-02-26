#! /bin/sh
set -ex

# Minor helper to avoid repeating myself,
# This shortcut allows you to simply invoke ../../autogen.sh when being
# in directories like:
#     ./target/{Debug,RelWithDebInfo,Release}
if [ -e ../../autogen.sh ] && [ "x$1" == "x" ] &&
   [ -x "$(command -v realpath)" ] &&
   [ "$(basename $(realpath ${PWD}/..))" = "target" ]
then
    exec ../../autogen.sh $(basename $(realpath ${PWD}))
fi

if [ -x "$(command -v realpath)" ]; then
    ROOTDIR="$(realpath $(dirname $0))"
else
    ROOTDIR="$(dirname $0)"
fi

BUILD_TYPE="${1:-Debug}"
BUILD_DIR="${ROOTDIR}/target/${BUILD_TYPE}"

echo "EXTRA_CMAKE_FLAGS: ${EXTRA_CMAKE_FLAGS}"

exec cmake "${ROOTDIR}" \
           -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
           ${EXTRA_CMAKE_FLAGS} \
           -B "${BUILD_DIR}" \
           -GNinja

