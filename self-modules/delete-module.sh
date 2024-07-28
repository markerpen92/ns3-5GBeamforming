#!/bin/bash

if [ -z "$1" ]; then
    echo "Usage: $0 <directory>"
    exit 1
fi

dir=${1%/}

if [ ! -d "$dir" ]; then
    echo "Directory $dir does not exist."
    exit 1
fi

result=$(find .. -type d -name "$dir")

filteredResult=()
for item in $result; do
    if [[ "$item" != *"self-module"* ]]; then
        filteredResult+=("$item")
    fi
done

mkdir -p ../trash

for item in "${filteredResult[@]}"; do
    if [[ "$item" == ../build/src/* ]]; then
        rm -rf "$item"
    else
        trash_item="../trash/$(basename "$item")"
        if [ -d "$trash_item" ]; then
            rm -rf "$trash_item"
        fi
        mv "$item" ../trash/
    fi
done

if [ -f ../enablelist.txt ]; then
    sed -i "/$dir/d" ../enablelist.txt
fi

cd ..
./GigConfig.sh
