#!/bin/bash
set -euo pipefail

FILES_USING_INCLUDE_GUARDS=$(
    find . -type f -name "*.h" -not -path "./build/*" | xargs grep -Pzl '#ifndef (.*)\n#define \1\n'
) || true
if [[ "$FILES_USING_INCLUDE_GUARDS" != "" ]]; then
    echo "The following files are using include guards but should use '#pragma once' instead!"
    echo "$FILES_USING_INCLUDE_GUARDS"
    exit 1
fi
