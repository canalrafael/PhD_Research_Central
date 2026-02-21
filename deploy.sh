#!/bin/bash

# 1. Define variables
LOCAL_PATH="$HOME/Desktop/git/PhD_Research_Central"
REMOTE_USER="root"
REMOTE_IP="fz3" # Uses 192.168.1.9 based on ~/.ssh/config

echo "--- Starting Deployment to FZ3 ---"

# 2. Cross-Compile Benchmarks for FZ3 (Cortex-A53)
# We compile on the VM because the FZ3 lacks a native 'gcc'
echo "Cross-compiling bandwidth benchmark..."
aarch64-linux-gnu-gcc \
    $LOCAL_PATH/external/misc-benchmarks/bandwidth.c \
    -o $LOCAL_PATH/external/misc-benchmarks/bandwidth_fz3 \
    -lpthread

# 3. Ensure the remote directory exists
ssh $REMOTE_USER@$REMOTE_IP "mkdir -p ~/PhD_Research_Central"

# 4. Sync files
# Copies all research code and the new cross-compiled binary to the board
echo "Syncing code via SCP..."
scp -r $LOCAL_PATH $REMOTE_USER@$REMOTE_IP:~/

# 5. Clear Cache (Essential for clean PhD data)
# Drops file system caches on the FZ3 to ensure benchmark accuracy
echo "Clearing FZ3 System Cache..."
ssh $REMOTE_USER@$REMOTE_IP "echo 3 > /proc/sys/vm/drop_caches"

# 6. Run Platform Setup
# Triggers the platform detection script on the board
echo "Running setup_platform.sh on FZ3..."
ssh $REMOTE_USER@$REMOTE_IP "cd ~/PhD_Research_Central && chmod +x scripts/setup_platform.sh && ./scripts/setup_platform.sh"

echo "--- FZ3 is Ready ---"