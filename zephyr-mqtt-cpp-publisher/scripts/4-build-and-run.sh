#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "========================================"
echo "  Build and Run MQTT Publisher"
echo "========================================"
echo ""

# Check networking is ready
echo "Checking network setup..."
if ! pgrep -f "loop-socat" > /dev/null; then
    echo "WARNING: socat not running!"
    echo "Start with: ./scripts/2-start-networking.sh"
    read -p "Continue anyway? (y/n) " -n 1 -r
    echo
    [[ ! $REPLY =~ ^[Yy]$ ]] && exit 1
fi

# Check broker is running
if ! pgrep -f "mosquitto" > /dev/null; then
    echo "WARNING: Mosquitto not running!"
    echo "Start with: ./scripts/3-start-broker.sh"
    read -p "Continue anyway? (y/n) " -n 1 -r
    echo
    [[ ! $REPLY =~ ^[Yy]$ ]] && exit 1
fi

cd "$PROJECT_ROOT"

# Build
echo ""
echo "Building for qemu_x86..."
west build -b qemu_x86 -p auto

echo ""
echo "========================================"
echo "  Build successful!"
echo "========================================"
echo ""
echo "To run: west build -t run"
echo "To exit QEMU: Ctrl+A then X"
echo ""
read -p "Run now? (y/n) " -n 1 -r
echo

if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo ""
    echo "Starting QEMU..."
    echo ""
    west build -t run
fi
