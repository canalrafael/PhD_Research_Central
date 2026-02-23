#!/bin/bash

# --- 1. CONFIGURATION ---
REMOTE_USER="root"
REMOTE_IP="fz3"
REMOTE_DIR="~/PhD_Research_Central"
LOCAL_PATH="$HOME/Desktop/git/PhD_Research_Central"

# Define the local cross-compiler tools
CC_AARCH64="aarch64-linux-gnu-gcc"
AR_AARCH64="aarch64-linux-gnu-ar"
CXX_AARCH64="aarch64-linux-gnu-g++"

echo "--- Starting Scalable PhD Deployment for FZ3 ---"

# --- 2. COMPILATION FUNCTIONS ---

build_attacks() {
    echo "[+] Compiling Attacks..."
    
    # 2.1 Armageddon
    if [ -d "$LOCAL_PATH/external/attacks/armageddon/libflush" ]; then
        cd "$LOCAL_PATH/external/attacks/armageddon/libflush" || exit
        make clean > /dev/null 2>&1
        # Added -fno-strict-aliasing to silence type-mismatch warnings
        make ARCH=armv8 CROSS_COMPILE=aarch64-linux-gnu- \
             CC="$CC_AARCH64 -fno-strict-aliasing -Wno-lto-type-mismatch" \
             AR=$AR_AARCH64 -j$(nproc)
    fi

    # # 2.2 Rowhammer.js
    # if [ -d "$LOCAL_PATH/external/attacks/rowhammerjs/native" ]; then
    #     echo "[+] Building Rowhammer Native Tools for ARM..."
    #     cd "$LOCAL_PATH/external/attacks/rowhammerjs/native" || exit
    #     $CXX_AARCH64 -O3 -std=c++11 rowhammer.cc -o rowhammer_fz3 -lpthread \
    #         -DARMV8 -DNO_X86_ASM 
    # fi # It is crashing my PC

    # 2.3 SpectrePoC
    if [ -d "$LOCAL_PATH/external/attacks/spectre" ]; then
        echo "[+] Building SpectrePoC for FZ3..."
        cd "$LOCAL_PATH/external/attacks/spectre" || exit
        # Use -O0 to ensure the compiler doesn't optimize away the vulnerability
        $CC_AARCH64 -O0 spectre.c -o spectre_fz3 
    fi

    # 2.4 Spectre_2
    if [ -d "$LOCAL_PATH/external/attacks/spectre_2" ]; then
        echo "[+] Building Spectre_2 (Research Toolkit) for FZ3..."
        cd "$LOCAL_PATH/external/attacks/spectre_2" || exit
        
        $CC_AARCH64 -O0 main.c util.c perf.c spectre_pht_sa_ip.c asm.c \
                    -o spectre_2_fz3 -lpthread
    fi
    
    cd "$LOCAL_PATH"
}

build_benchmarks() {
    echo "[+] Compiling Benchmarks..."
    # Using your Ubuntu cross-compiler for the bandwidth tool
    $CC_AARCH64 "$LOCAL_PATH/external/benchmarks/misc-benchmarks/bandwidth.c" \
                -o "$LOCAL_PATH/external/benchmarks/misc-benchmarks/bandwidth_fz3" \
                -lpthread
    
    # Compile the latency tool
    $CC_AARCH64 "$LOCAL_PATH/external/benchmarks/misc-benchmarks/latency.c" \
                -o "$LOCAL_PATH/external/benchmarks/misc-benchmarks/latency_fz3" \
                -lpthread
    
    if [ -d "$LOCAL_PATH/external/benchmarks/sort-bench" ]; then
        echo "[+] Building Sort Benchmark for FZ3 (Full Link)..."
        cd "$LOCAL_PATH/external/benchmarks/sort-bench" || exit
        
        $CXX_AARCH64 -O3 bench.cpp std-sorts.cpp msort-copy.cpp radix1.cpp \
                    radix2.cpp radix3.cpp radix4.cpp radix5.cpp radix6.cpp \
                    -o bench -lpthread
    fi

    cd "$LOCAL_PATH"
}

# Run the builds
build_attacks
build_benchmarks

# --- 3. SYNCHRONIZATION ---
echo "[+] Syncing structure to FZ3 (Excluding videos and objects)..."
tar --exclude='*.mp4' --exclude='*.o' --exclude='.git' -cf - external/ scripts/ | \
    ssh "$REMOTE_USER@$REMOTE_IP" "mkdir -p $REMOTE_DIR && tar -xf - -C $REMOTE_DIR"

# --- 4. REMOTE SETUP ---
echo "[+] Finalizing Board Configuration..."
ssh "$REMOTE_USER@$REMOTE_IP" << EOF
    # Smart permission update: only grant +x to actual binaries, not source code
    find $REMOTE_DIR/external -type f -executable -exec chmod +x {} +
    chmod +x $REMOTE_DIR/scripts/*.sh
    
    echo 1 > /proc/sys/kernel/perf_event_paranoid
    sync; echo 3 > /proc/sys/vm/drop_caches
    echo "Board ready for experiments."
EOF

echo "--- Deployment Complete ---"