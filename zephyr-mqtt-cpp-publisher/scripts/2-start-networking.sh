#!/bin/bash

echo "========================================"
echo "  Virtual Network Setup"
echo "========================================"
echo ""
echo "This script starts TWO processes:"
echo "  1. SOCAT (this terminal)"
echo "  2. SLIP-TAP bridge (needs sudo in another terminal)"
echo ""
echo "IMPORTANT: After this starts, open another terminal and run:"
echo "  sudo ~/zephyrproject/net-tools/loop-slip-tap.sh"
echo ""
echo "Press Ctrl+C to stop"
echo "========================================"
echo ""

# Check if net-tools exists
if [ ! -d "$HOME/zephyrproject/net-tools" ]; then
    echo "ERROR: net-tools not found!"
    echo "Run: ./scripts/1-setup-workspace.sh first"
    exit 1
fi

cd ~/zephyrproject/net-tools

# Kill any existing processes
pkill -f loop-socat || true

# Start socat
./loop-socat.sh
