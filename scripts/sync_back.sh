#!/bin/bash

# 1. Define variables
REMOTE_USER="root"
REMOTE_IP="fz3" 
REMOTE_PROJECT_PATH="~/PhD_Research_Central/"
LOCAL_PROJECT_PATH="$HOME/Desktop/git/PhD_Research_Central/"

echo "--- Synchronizing F.Z.3 Work back to V.M. ---"

# 2. Sync files back using scp
# We use -r for recursive and -p to preserve modification times/permissions
# This pulls the entire directory structure including data/raw/ and modified scripts
echo "Pulling updates from F.Z.3 via SCP..."
scp -rp $REMOTE_USER@$REMOTE_IP:$REMOTE_PROJECT_PATH* $LOCAL_PROJECT_PATH

if [ $? -eq 0 ]; then
    echo "Sync successful. Files are now in $LOCAL_PROJECT_PATH"
    
    # 3. Show status of retrieved research data
    echo "Latest P.M.U. results retrieved:"
    ls -lh ${LOCAL_PROJECT_PATH}data/raw/ | tail -n 5
else
    echo "Error: Synchronization failed. Check your S.S.H. connection."
fi

echo "--- Sync Complete ---"