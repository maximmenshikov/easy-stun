#!/bin/bash
export TOP_DIR="$(cd "$(dirname "$(which "$0")")"/.. ; pwd -P)"

pushd "${TOP_DIR}"

if [ -f ".build_wrap" ] ; then
    ./.build_wrap
else
    mkdir -p build
    pushd build
        cmake -GNinja ..
        ninja
    popd
fi

popd
