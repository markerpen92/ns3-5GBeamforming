#!/bin/bash

# Ensure enablelist.txt exists
if [ ! -f "enablelist.txt" ]; then
  echo "enablelist.txt does not exist."
  exit 1
fi

while IFS= read -r module; do
  # Skip empty lines or lines starting with #
  if [[ -z "$module" || "$module" =~ ^# ]]; then
    continue
  fi

  # Check if module directory exists in NS3-WiGig/src
  if [ ! -d "../../NS3-WiGig/src/$module" ]; then
    echo "$module does not exist in NS3-WiGig/src/$module, skipping."
    continue
  fi

  # Check if module directory exists in ns-3-dev-git/src
  if [ ! -d "../../ns-3-dev/src/$module" ]; then
    echo "$module does not exist in ns-3-dev/src/$module, skipping."
    continue
  fi

  mkdir -p "$module"

  for dir in $(ls "../../NS3-WiGig/src/$module"); do
    if [ -d "../../NS3-WiGig/src/$module/$dir" ]; then
      # Check if target directory exists in ns-3-dev-git
      if [ -d "../../ns-3-dev/src/$module/$dir" ]; then
        mkdir -p "./$module/$dir"

        # Generate diff file
        diff "../../NS3-WiGig/src/$module/$dir" "../../ns-3-dev/src/$module/$dir" > "./$module/$dir/$module-$dir-log.txt"
      else
        echo "Skipping $module/$dir as it does not exist in ns-3-dev."
      fi
    fi
  done
done < enablelist.txt

