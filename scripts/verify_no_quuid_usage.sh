#!/bin/bash
set -euo pipefail

FILES_USING_QUUID_UNINTENTIONALLY=$(grep -Rl QUuid src/ | grep -v "src/app/backend/dataitems/qtdid.[h|cpp]") || true
if [[ "$FILES_USING_QUUID_UNINTENTIONALLY" != "" ]]; then
    echo "The following files are using QUuid but should not. Use QtdId instead!"
    echo "$FILES_USING_QUUID_UNINTENTIONALLY"
    exit 1
fi
