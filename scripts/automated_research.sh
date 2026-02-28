#!/bin/bash

# --- 1. CONFIGURATION AND ARGUMENTS ---
# Usage: sudo ./scripts/automated_research.sh <MINUTES_PER_ITEM> <MODE: --parallel | --sequential>
# sudo ./scripts/automated_research.sh 30 --sequential
# sudo ./scripts/automated_research.sh 30 --parallel

# cd ~/PhD_Research_Central
# nohup sudo ./scripts/automated_research.sh 30 --sequential > research_feb28.log 2>&1 &
# disown

MINUTES_PER_ITEM=${1:-10}
MODE=${2:-"--sequential"} 
SECONDS_PER_ITEM=$((MINUTES_PER_ITEM * 60))

BASE_DIR="$HOME/PhD_Research_Central"
DATA_RAW_DIR="$BASE_DIR/data/raw"
SPECTRE_THRESHOLD=4

mkdir -p "$DATA_RAW_DIR"

# --- 2. HELPER FUNCTIONS ---

# Reset hardware state to ensure experimental consistency
reset_system() {
    echo "[+] Resetting Hardware State..."
    sync; echo 3 > /proc/sys/vm/drop_caches
    echo 1 > /proc/sys/kernel/perf_event_paranoid
}

# Fixed PMU monitoring function using perf stat
monitor_pmu_fixed() {
    local label=$1
    local duration=$2
    local timestamp=$(date +%Y%m%d_%H%M%S)
    local output_csv="${DATA_RAW_DIR}/${label}_${timestamp}.csv"
    local end_time=$(( $(date +%s) + duration ))
    
    echo "timestamp,cycles,instructions,cache_misses,cache_references,branch_misses,bus_cycles,branches" > "$output_csv"

    echo "[*] Monitoring $label for $duration seconds..."
    while [ $(date +%s) -lt $end_time ]; do
        # Collect hardware counters every 250ms
        local STATS=$(perf stat -a -e cycles,instructions,cache-misses,cache-references,branch-misses,bus-cycles,branches sleep 0.25 2>&1)
        
        local CYC=$(echo "$STATS" | awk '/cycles/ {print $1}' | head -1 | tr -d ',')
        local INS=$(echo "$STATS" | awk '/instructions/ {print $1}' | tr -d ',')
        local CMS=$(echo "$STATS" | awk '/cache-misses/ {print $1}' | tr -d ',')
        local CRF=$(echo "$STATS" | awk '/cache-references/ {print $1}' | tr -d ',')
        local BMS=$(echo "$STATS" | awk '/branch-misses/ {print $1}' | tr -d ',')
        local BUS=$(echo "$STATS" | awk '/bus-cycles/ {print $1}' | tr -d ',')
        local BRA=$(echo "$STATS" | awk '/ branches/ {print $1}' | tr -d ',')
        local TS=$(date +"%H:%M:%S.%3N")

        echo "$TS,$CYC,$INS,$CMS,$CRF,$BMS,$BUS,$BRA" >> "$output_csv"
    done
}

# --- 3. TARGET COMMANDS ---
BENCH_CMDS=(
    "bandwidth:$BASE_DIR/external/benchmarks/misc-benchmarks/bandwidth_fz3 -m 100 -t 5"
    # "latency:$BASE_DIR/external/benchmarks/misc-benchmarks/latency_fz3"
    "sort_bench:$BASE_DIR/external/benchmarks/sort-bench/bench"
)

ATTACK_CMDS=(
    "spectre_poc:$BASE_DIR/external/attacks/spectre/spectre_fz3 $SPECTRE_THRESHOLD"
    "spectre_v2:$BASE_DIR/external/attacks/spectre_2/spectre_2_fz3 -l 200 -c $SPECTRE_THRESHOLD"
    "meltdown:$BASE_DIR/external/attacks/meltdown/test"
    "armageddon:$BASE_DIR/external/attacks/armageddon/libflush/build/armv8/release/example"
)

# --- 4. EXECUTION ---

if [ "$MODE" == "--parallel" ]; then
    # PARALLEL MODE: Run Benchmarks and Attacks simultaneously on separate cores
    for B_ENTRY in "${BENCH_CMDS[@]}"; do
        IFS=":" read -r B_NAME B_PATH <<< "$B_ENTRY"
        for A_ENTRY in "${ATTACK_CMDS[@]}"; do
            IFS=":" read -r A_NAME A_PATH <<< "$A_ENTRY"
            reset_system
            echo "[!] Starting Pair: $B_NAME + $A_NAME"
            
            # Execute both targets in an infinite loop in the background
            ( while true; do taskset -c 0 $B_PATH > /dev/null 2>&1; done ) & PID_B=$!
            ( while true; do taskset -c 1 $A_PATH > /dev/null 2>&1; done ) & PID_A=$!
            
            monitor_pmu_fixed "parallel_${B_NAME}_${A_NAME}" $SECONDS_PER_ITEM
            
            kill -9 $PID_B $PID_A 2>/dev/null
            wait $PID_B $PID_A 2>/dev/null
        done
    done
else
    # SEQUENTIAL MODE: Run each target individually
    for B_ENTRY in "${BENCH_CMDS[@]}"; do
        IFS=":" read -r B_NAME B_PATH <<< "$B_ENTRY"
        reset_system
        ( while true; do taskset -c 0 $B_PATH > /dev/null 2>&1; done ) & PID_W=$!
        monitor_pmu_fixed "$B_NAME" $SECONDS_PER_ITEM
        kill -9 $PID_W 2>/dev/null
        wait $PID_W 2>/dev/null
    done

    for A_ENTRY in "${ATTACK_CMDS[@]}"; do
        IFS=":" read -r A_NAME A_PATH <<< "$A_ENTRY"
        reset_system
        ( while true; do taskset -c 0 $A_PATH > /dev/null 2>&1; done ) & PID_W=$!
        monitor_pmu_fixed "$A_NAME" $SECONDS_PER_ITEM
        kill -9 $PID_W 2>/dev/null
        wait $PID_W 2>/dev/null
    done
fi

echo "=== Research Completed. Data saved in $DATA_RAW_DIR ==="