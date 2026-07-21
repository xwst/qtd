#!/bin/bash
set -euxo pipefail

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 BUILD_DIRECTORY"
    exit 1
fi


BUILD_DIR=$1
FILES=$(
	git diff --name-status origin/main \
	| egrep "\.cpp$" \
	| grep -v "^D" \
	| awk '{print $NF}'
) || true

if [ "$FILES" != "" ]; then
    echo "Run static code analysis on the following files:"
    echo $FILES | tr ' ' '\n'
    ${CLANG_TIDY:-run-clang-tidy} -p $BUILD_DIR -config-file .clang-tidy -j 4 $FILES
else
    echo "No files to check."
fi
