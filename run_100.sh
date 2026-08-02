#!/usr/bin/env bash

set -euo pipefail
export LC_ALL=C

RUNS=100
MAIN_SOURCE="main.c"
BINVEC_SOURCE="binvec.c"
MAIN_EXE="./main"
BINVEC_EXE="./binvec"
RAW_FILE="raw_values.csv"
DIST_FILE="value_distribution.csv"
BENCHMARK_FILE="benchmark.txt"

cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

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

if [[ ! -f "$MAIN_SOURCE" ]]; then
  printf 'Missing file: %s\n' "$MAIN_SOURCE" >&2
  exit 1
fi

if [[ ! -f "$BINVEC_SOURCE" ]]; then
  printf 'Missing file: %s\n' "$BINVEC_SOURCE" >&2
  exit 1
fi

for data_file in "${DATA_FILES[@]}"; do
  if [[ ! -f "$data_file" ]]; then
    printf 'Missing file: %s\n' "$data_file" >&2
    exit 1
  fi
done

printf 'Compiling programs...\n'
gcc -Wall -Wextra -O2 -o "$BINVEC_EXE" "$BINVEC_SOURCE"
gcc -Wall -Wextra -O2 -o "$MAIN_EXE" "$MAIN_SOURCE"

printf 'Running binvec benchmark five times...\n'
binvec_times=()
T="0"

for run_number in 1 2 3 4 5; do
  binvec_output=$("$BINVEC_EXE")
  time_value=$(printf '%s\n' "$binvec_output" | awk '$1 == "time" && $2 == "=" { print $3; exit }')

  if [[ ! "$time_value" =~ ^[0-9]+([.][0-9]+)?$ ]]; then
    printf 'Failed to read binvec time on run %d.\n' "$run_number" >&2
    exit 1
  fi

  binvec_times+=("$time_value")
  T=$(awk -v a="$T" -v b="$time_value" 'BEGIN { printf "%.6f", a + b }')
  printf '  Run %d: %s\n' "$run_number" "$time_value"
done

THREE_T=$(awk -v t="$T" 'BEGIN { printf "%.6f", 3 * t }')

{
  echo "Binvec times:"
  for i in "${!binvec_times[@]}"; do
    printf 'Run %d: %s\n' "$((i + 1))" "${binvec_times[$i]}"
  done
  printf 'T: %s\n' "$T"
  printf '3T: %s\n' "$THREE_T"
} > "$BENCHMARK_FILE"

printf 'T = %s\n' "$T"
printf '3T = %s\n' "$THREE_T"

printf 'run,file,value\n' > "$RAW_FILE"

for ((run_number = 1; run_number <= RUNS; run_number++)); do
  printf 'Run %d/%d\n' "$run_number" "$RUNS"

  for data_file in "${DATA_FILES[@]}"; do
    time_limit="$T"
    if [[ "$data_file" == DataFiles/data162-*.txt ]]; then
      time_limit="$THREE_T"
    fi

    if ! program_output=$(printf '%s\n' "$data_file" | "$MAIN_EXE" "$time_limit"); then
      printf 'Main program failed. Run=%d File=%s\n' "$run_number" "$data_file" >&2
      exit 1
    fi

    value=$(printf '%s\n' "$program_output" | awk '$1 == "Value:" { print $2; exit }')

    if [[ ! "$value" =~ ^-?[0-9]+$ ]]; then
      printf 'Failed to read Value. Run=%d File=%s\n' "$run_number" "$data_file" >&2
      printf '%s\n' "$program_output" >&2
      exit 1
    fi

    printf '%d,%s,%s\n' "$run_number" "$data_file" "$value" >> "$RAW_FILE"
    printf '  %-29s Value=%s\n' "$data_file" "$value"
  done
done

tmp_distribution=$(mktemp)
trap 'rm -f "$tmp_distribution"' EXIT

awk -F, -v runs="$RUNS" '
  NR == 1 { next }
  {
    key = $2 SUBSEP $3
    count[key]++
  }
  END {
    for (key in count) {
      split(key, parts, SUBSEP)
      printf "%s,%s,%d,%.2f\n", parts[1], parts[2], count[key], 100.0 * count[key] / runs
    }
  }
' "$RAW_FILE" > "$tmp_distribution"

{
  echo "file,value,count,percentage"
  sort -t, -k1,1 -k2,2n "$tmp_distribution"
} > "$DIST_FILE"

rm -f "$tmp_distribution"
trap - EXIT

printf 'Finished.\n'
printf 'Raw values: %s\n' "$RAW_FILE"
printf 'Distribution: %s\n' "$DIST_FILE"
printf 'Benchmark: %s\n' "$BENCHMARK_FILE"
