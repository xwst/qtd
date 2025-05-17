#!/bin/bash
set -euxo pipefail

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 BUILD_DIRECTORY"
    exit 1
fi


BUILD_DIR=$1
FILES=$(git diff --name-only origin/main | egrep "\.h$|\.cpp$") || true

if [ "$FILES" != "" ]; then
    echo "Run static code analysis on the following files:"
    echo $FILES | tr ' ' '\n'
    exit $(clang-tidy -p $BUILD_DIR --config-file .clang-tidy $FILES)
else
    echo "No files to check."
fi
