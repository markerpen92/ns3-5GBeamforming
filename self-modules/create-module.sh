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

python3 ../src/create-module.py "$dir"

mkdir -p "../src/$dir/model"
cp -r "$dir"/* "../src/$dir/model"

files=()

while IFS= read -r -d '' file; do
    if [[ "$file" == *.cc || "$file" == *.h ]]; then
        relative_path="model/${file#"$dir/"}"
        files+=("$relative_path")
    fi
done < <(find "$dir" -type f \( -name "*.cc" -o -name "*.h" \) -print0)

# Add default files only if they do not exist in files array
if ! [[ " ${files[*]} " == *" model/$dir.cc "* ]]; then
    files+=("model/$dir.cc")
fi

if ! [[ " ${files[*]} " == *" model/$dir.h "* ]]; then
    files+=("model/$dir.h")
fi

echo "Files to be included in wscript: ${files[*]}"

wscript_path="../src/$dir/wscript"
echo "Generating wscript file at $wscript_path"

{
    echo "# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-"
    echo ""
    echo "def build(bld):"
    echo "    module = bld.create_ns3_module('$dir', ['core'])"
    echo "    module.source = ["
    for file in "${files[@]}"; do
        if [[ "$file" == *.cc ]]; then
            echo "        '$file',"
        fi
    done
    echo "    ]"
    echo ""
    echo "    module_test = bld.create_ns3_module_test_library('$dir')"
    echo "    module_test.source = ["
    echo "        'test/app-install-test-suite.cc',"
    echo "    ]"
    echo ""
    echo "    headers = bld(features='ns3header')"
    echo "    headers.module = '$dir'"
    echo "    headers.source = ["
    for file in "${files[@]}"; do
        if [[ "$file" == *.h ]]; then
            echo "        '$file',"
        fi
    done
    echo "    ]"
    echo ""
    echo "    if bld.env.ENABLE_EXAMPLES:"
    echo "        bld.recurse('examples')"
    echo ""
    echo "    # bld.ns3_python_bindings()"
} > "$wscript_path"

echo "wscript file generated successfully."

if ! grep -Fxq "$dir" ../enablelist.txt; then
    echo "$dir" >> ../enablelist.txt
    echo "$dir added to enablelist.txt"
else
    echo "$dir is already in enablelist.txt"
fi

cd ..
./GigConfig.sh
