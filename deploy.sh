#!/bin/bash

# 1. Sync the code
echo "Syncing code to FZ3..."
rsync -avz --exclude '.git' . fz3:~/PhD_Research_Central/

# 2. Clear Cache on FZ3 (Remote Command)
# We use 'drop_caches' to clear the OS page cache
echo "Clearing FZ3 System Cache..."
ssh fz3 "echo 3 > /proc/sys/vm/drop_caches"

# 3. Setup Platform
echo "Configuring FZ3 environment..."
ssh fz3 "cd ~/PhD_Research_Central && ./scripts/setup_platform.sh"

echo "FZ3 is clean and ready for Benchmark."
