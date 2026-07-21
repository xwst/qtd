#!/bin/bash
set -euxo pipefail

FILES=$(
	git diff --name-status origin/main \
	| egrep "\.qml$" \
	| grep -v "^D" \
	| awk '{print $NF}'
) || true

QMLLINT=$(command -v qmllint) || QMLLINT=/usr/lib/qt6/bin/qmllint

IMPORT_PATH=""
if [ -n "${1-}" ]; then
    IMPORT_PATH="-I $1"
fi

if [ "$FILES" != "" ]; then
    echo "Run qmllint on the following files:"
    echo "$FILES" | tr ' ' '\n'
    $QMLLINT $IMPORT_PATH $FILES
else
    echo "No QML files to check."
fi
