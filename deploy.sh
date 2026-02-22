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
    
    # 2.1 Armageddon (libflush)
    if [ -d "$LOCAL_PATH/external/attacks/armageddon/libflush" ]; then
        cd "$LOCAL_PATH/external/attacks/armageddon/libflush" || exit
        make clean > /dev/null 2>&1
        make ARCH=armv8 CROSS_COMPILE=aarch64-linux-gnu- CC=$CC_AARCH64 AR=$AR_AARCH64 -j$(nproc)
    else
        echo "[!] Warning: Armageddon (libflush) path not found."
    fi

    # 2.2 Rowhammer.js (Native Tools)
    if [ -d "$LOCAL_PATH/external/attacks/rowhammerjs/native" ]; then
        echo "[+] Building Rowhammer Native Tools for ARM..."
        cd "$LOCAL_PATH/external/attacks/rowhammerjs/native" || exit
        
        # We use a flag to disable x86-specific assembly if the source supports it,
        # or we compile the ARM-specific version if available.
        # For Rowhammerjs, we often need to define the architecture.
        $CXX_AARCH64 -O3 -std=c++11 rowhammer.cc -o rowhammer_fz3 -lpthread \
            -DARMV8 -DNO_X86_ASM 
    fi
    
    cd "$LOCAL_PATH"
}

build_benchmarks() {
    echo "[+] Compiling Benchmarks..."
    # Using your Ubuntu cross-compiler for the bandwidth tool
    $CC_AARCH64 "$LOCAL_PATH/external/benchmarks/misc-benchmarks/bandwidth.c" \
                -o "$LOCAL_PATH/external/benchmarks/misc-benchmarks/bandwidth_fz3" \
                -lpthread
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