#!/bin/bash

LICENSE_HEADER_HASH="cfefb9fb659e240c24efbc82e6c49571073c1c9cdae0553255bbdf2065023e41"

RESULT=0

FILE=$(find . \( -name "*.cpp" -o -name "*.h" \) -not -path "./build/*")
while read FILE_NAME; do
	HASH=$(grep -A12 "This file is part of qtd" "$FILE_NAME" | sha256sum.exe)
	if [ "$HASH" != "$LICENSE_HEADER_HASH" ]; then
		echo "$FILE_NAME is not properly licensed!"
		RESULT=1
	fi
done < <(find . \( -name "*.cpp" -o -name "*.h" \) -not -path "./build/*")

exit $RESULT
