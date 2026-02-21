#!/bin/bash

# Detect the CPU model
CPU_MODEL=$(grep "model name" /proc/cpuinfo | head -1)

echo "Detecting Hardware..."

if [[ "$CPU_MODEL" == *"Cortex-A72"* ]]; then
    echo "Hardware Detected: Raspberry Pi 4 (Cortex-A72)"
    PLATFORM="raspi4"
elif [[ "$CPU_MODEL" == *"Cortex-A53"* ]]; then
    echo "Hardware Detected: FZ3 Board (Cortex-A53)"
    PLATFORM="fz3"
else
    # Force FZ3 as the default for your PhD environment
    echo "Unknown Hardware ($CPU_MODEL). Manually selecting FZ3 for PhD Research."
    PLATFORM="fz3"
fi

# Switch Submodule Branches
echo "Switching submodules to platform/$PLATFORM..."
cd external/meltdown
git checkout platform/$PLATFORM
cd ../..

echo "Platform setup complete for $PLATFORM."
