#!/bin/bash

# 1. Configuration
# Usage: ./scripts/pmu_monitor.sh <benchmark_name>
BENCH_NAME=${1:-"bandwidth"} 
DATA_DIR="data/raw"
DURATION=3600  # 1 hour
INTERVAL=1     # 1 second sampling
TIMESTAMP=$(date +%Y%m%d_%H%M%S)

# Filename requirements: saved in data/raw, benchmark name, and no-attack label
OUTPUT_FILE="${DATA_DIR}/pmu_${BENCH_NAME}_noattack_${TIMESTAMP}.csv"

# Ensure the research data folder exists on the FZ3
mkdir -p "$DATA_DIR"

echo "--- Starting Experiment: $BENCH_NAME (Baseline/No Attack) ---"
echo "Results will be saved to: $OUTPUT_FILE"

# 2. Add CSV Header for all 7 available Cortex-A53 counters
echo "timestamp,cycles,instructions,cache_misses,cache_references,branch_misses,bus_cycles,branches" > "$OUTPUT_FILE"

# 3. Select and Start the specified Benchmark
if [ "$BENCH_NAME" == "bandwidth" ]; then
    # Cross-compiled bandwidth tool location
    CMD="./external/misc-benchmarks/bandwidth_fz3 -m 100 -t 3600"
elif [ "$BENCH_NAME" == "sort" ]; then
    # Sort benchmark from your benchmarking suite
    CMD="./scripts/benchmarking/sort-bench/bench"
else
    echo "Using generic command for: $BENCH_NAME"
    CMD="./$BENCH_NAME"
fi

echo "Executing benchmark: $CMD"
$CMD > /dev/null 2>&1 &
BENCH_PID=$!

# 4. Monitoring Loop
for ((i=1; i<=DURATION; i++)); do
    # Capture the 7 hardware events confirmed active in your PMU driver
    STATS=$(perf stat -a -e cycles,instructions,cache-misses,cache-references,branch-misses,bus-cycles,branches sleep $INTERVAL 2>&1)
    
    # Parse values and remove formatting commas for CSV integrity
    CYC=$(echo "$STATS" | awk '/cycles/ {print $1}' | head -1 | tr -d ',')
    INS=$(echo "$STATS" | awk '/instructions/ {print $1}' | tr -d ',')
    CMS=$(echo "$STATS" | awk '/cache-misses/ {print $1}' | tr -d ',')
    CRF=$(echo "$STATS" | awk '/cache-references/ {print $1}' | tr -d ',')
    BMS=$(echo "$STATS" | awk '/branch-misses/ {print $1}' | tr -d ',')
    BUS=$(echo "$STATS" | awk '/bus-cycles/ {print $1}' | tr -d ',')
    BRA=$(echo "$STATS" | awk '/ branches/ {print $1}' | tr -d ',')
    TS=$(date +"%H:%M:%S")

    echo "$TS,$CYC,$INS,$CMS,$CRF,$BMS,$BUS,$BRA" >> "$OUTPUT_FILE"

    # Log progress to terminal every minute
    if (( i % 60 == 0 )); then
        echo "Data Collection: $((i / 60)) minutes completed."
    fi
    
    # Exit if the benchmark finishes before the hour is up
    if ! kill -0 $BENCH_PID 2>/dev/null; then 
        echo "Benchmark process finished. Ending collection."
        break 
    fi
done

echo "Experiment successful. Data archived in $OUTPUT_FILE"