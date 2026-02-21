#!/bin/bash

# Detect Hardware
CPU_MODEL=$(grep "model name" /proc/cpuinfo | head -1)
echo "Detecting Hardware..."

if [[ "$CPU_MODEL" == *"Cortex-A72"* ]]; then
    echo "Hardware Detected: Raspberry Pi 4 (Cortex-A72)"
    PLATFORM="raspi4"
elif [[ "$CPU_MODEL" == *"Cortex-A53"* ]]; then
    echo "Hardware Detected: FZ3 Board (Cortex-A53)"
    PLATFORM="fz3"
else
    # Default to fz3 for PhD Research as a fallback
    echo "Unknown Hardware ($CPU_MODEL). Manually selecting FZ3 for PhD Research."
    PLATFORM="fz3"
fi

# 1. Check if git is installed before trying to switch branches
if command -v git >/dev/null 2>&1; then
    echo "Switching submodules to platform/$PLATFORM..."
    cd external/meltdown && git checkout platform/$PLATFORM && cd ../..
else
    # 2. Skip git command on the FZ3 board to avoid the error
    echo "Git not found on this system. Skipping submodule branch switch."
    echo "Ensure the correct branch was checked out on the host VM before deployment."
fi

echo "Platform setup complete for $PLATFORM."