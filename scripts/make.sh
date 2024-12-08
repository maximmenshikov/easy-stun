#!/bin/bash
export TOP_DIR="$(cd "$(dirname "$(which "$0")")"/.. ; pwd -P)"

cd "${TOP_DIR}"

if [ -f ".build_wrap" ] ; then
    ./.build_wrap
else
    make
fi
