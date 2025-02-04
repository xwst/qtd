#!/bin/bash

cd $(dirname $0)/..
find src/ test/ \
    | egrep "*.h$|*.cpp$" \
    | xargs clang-tidy -p build/ --config-file .clang-tidy
