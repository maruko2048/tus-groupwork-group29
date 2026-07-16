#!/usr/bin/env bash

set -u

EXE="./main"
RESULT_FILE="results.txt"

gcc -Wall -Wextra -O2 -o "$EXE" main.c

if [ $? -ne 0 ]; then
  echo "Compile failed."
  exit 1
fi

echo "Results" > "$RESULT_FILE"

run() {
  file="$1"

  echo "Running: $file"

  {
    echo "========================================"
    echo "Running: $file"
    printf '%s\n' "$file" | "$EXE"
  } >> "$RESULT_FILE"
}

run DataFiles/data8.txt
run DataFiles/data14-1.txt
run DataFiles/data14-2.txt
run DataFiles/data17-1.txt
run DataFiles/data17-2.txt
run DataFiles/data50-1.txt
run DataFiles/data50-2.txt
run DataFiles/data50-3.txt
run DataFiles/data50-4.txt
run DataFiles/data100-1.txt
run DataFiles/data100-2.txt
run DataFiles/data100-3.txt
run DataFiles/data100-4.txt
run DataFiles/data162-1.txt
run DataFiles/data162-2.txt
run DataFiles/data162-3.txt
run DataFiles/data162-4.txt

cat "$RESULT_FILE"

echo "========================================"
echo "Finished."