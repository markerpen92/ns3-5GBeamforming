#!/bin/bash

# Read enable modules from enablelist.txt
ENABLE_MODULES=""
if [[ -f enablelist.txt ]]; then
  while IFS= read -r line; do
    if [[ -n "$line" ]]; then
      if [[ -z "$ENABLE_MODULES" ]]; then
        ENABLE_MODULES="$line"
      else
        ENABLE_MODULES="$ENABLE_MODULES,$line"
      fi
    fi
  done < enablelist.txt
fi

# Configure and build ns-3
./waf configure --disable-examples --disable-tests --disable-python --enable-modules="$ENABLE_MODULES"
./waf build

