#!/bin/bash
set -euxo pipefail

LICENSE_HEADER_HASH="cfefb9fb659e240c24efbc82e6c49571073c1c9cdae0553255bbdf2065023e41"

RESULT=0
FOUND_MAIN=1

FILE=$(find . \( -name "*.cpp" -o -name "*.h" \) -not -path "./build/*")
while read FILE_NAME; do
    if [[ $FILE_NAME == *main.cpp ]]; then
        FOUND_MAIN=0
    fi
    HASH=$(grep -A12 "This file is part of qtd" "$FILE_NAME" | sha256sum | cut -d' ' -f1)
    if [ "$HASH" != "$LICENSE_HEADER_HASH" ]; then
        echo "$FILE_NAME is not properly licensed!"
        RESULT=1
    fi
done < <(find . \( -name "*.cpp" -o -name "*.h" \) -not -path "./build/*")

if [[ $FOUND_MAIN -eq 1 ]]; then
    echo "Was not able to find 'main.cpp'. Did you execute the script in the correct directory?"
    exit $FOUND_MAIN
fi
exit $RESULT
