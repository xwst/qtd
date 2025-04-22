#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 SOURCE_DIRECTORY BUILD_DIRECTORY"
    exit 1
fi


SRC_DIR=$1
BUILD_DIR=$2

find $SRC_DIR/src/ $SRC_DIR/test/ \
    | egrep "\.h$|\.cpp$" \
    | xargs clang-tidy -p $BUILD_DIR --config-file .clang-tidy
