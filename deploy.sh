#!/bin/bash

# 1. Define variables
LOCAL_PATH="$HOME/Desktop/git/PhD_Research_Central"
REMOTE_USER="root"
REMOTE_IP="fz3" # This works because of your ~/.ssh/config

echo "--- Starting Deployment to FZ3 ---"

# 2. Ensure the remote directory exists
ssh $REMOTE_USER@$REMOTE_IP "mkdir -p ~/PhD_Research_Central"

# 3. Sync files (The 'scp' way that avoids the '.' error)
# We copy the contents of the local folder to the remote folder
echo "Syncing code via SCP..."
scp -r $LOCAL_PATH $REMOTE_USER@$REMOTE_IP:~/

# 4. Clear Cache (Essential for clean PhD data)
echo "Clearing FZ3 System Cache..."
ssh $REMOTE_USER@$REMOTE_IP "echo 3 > /proc/sys/vm/drop_caches"

# 5. Run Platform Setup
echo "Running setup_platform.sh on FZ3..."
ssh $REMOTE_USER@$REMOTE_IP "cd ~/PhD_Research_Central && chmod +x scripts/setup_platform.sh && ./scripts/setup_platform.sh"

echo "--- FZ3 is Ready ---"
