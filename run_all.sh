#!/usr/bin/env bash

set -euo pipefail

BINVEC_EXE="./binvec"
MAIN_EXE="./main"
MAIN_SOURCE="main.c"
RESULT_FILE="results.txt"

DATA_FILES=(
  "DataFiles/data8.txt"
  "DataFiles/data14-1.txt"
  "DataFiles/data14-2.txt"
  "DataFiles/data17-1.txt"
  "DataFiles/data17-2.txt"
  "DataFiles/data50-1.txt"
  "DataFiles/data50-2.txt"
  "DataFiles/data50-3.txt"
  "DataFiles/data50-4.txt"
  "DataFiles/data100-1.txt"
  "DataFiles/data100-2.txt"
  "DataFiles/data100-3.txt"
  "DataFiles/data100-4.txt"
  "DataFiles/data162-1.txt"
  "DataFiles/data162-2.txt"
  "DataFiles/data162-3.txt"
  "DataFiles/data162-4.txt"
)

gcc -Wall -Wextra -O2 -o binvec binvec.c
gcc -Wall -Wextra -O2 -o main "$MAIN_SOURCE"

binvec_times=()
for run_number in 1 2 3 4 5; do
  binvec_output=$("$BINVEC_EXE")
  time_value=$(printf '%s\n' "$binvec_output" | awk '$1 == "time" && $2 == "=" { print $3; exit }')

  if [[ -z "$time_value" ]]; then
    printf 'Failed to read time from binvec run %d.\n' "$run_number" >&2
    exit 1
  fi

  binvec_times+=("$time_value")
done

T=$(awk 'BEGIN { for (i = 1; i < ARGC; i++) total += ARGV[i]; printf "%.6f", total }' "${binvec_times[@]}")
THREE_T=$(awk -v t="$T" 'BEGIN { printf "%.6f", 3 * t }')

{
  echo "Results"
  echo "Binvec times:"
  for i in "${!binvec_times[@]}"; do
    printf 'Run %d: %s\n' "$((i + 1))" "${binvec_times[$i]}"
  done
  printf 'T: %s\n' "$T"
} | tee "$RESULT_FILE"

for data_file in "${DATA_FILES[@]}"; do
  time_limit="$T"
  if [[ "$data_file" == DataFiles/data162-*.txt ]]; then
    time_limit="$THREE_T"
  fi

  {
    echo
    echo "========================================"
    echo "Running: $data_file"
  } | tee -a "$RESULT_FILE"

  printf '%s\n' "$data_file" \
    | "$MAIN_EXE" "$time_limit" \
    | sed '1s/^input filename: //' \
    | tee -a "$RESULT_FILE"
done
