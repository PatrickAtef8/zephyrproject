#!/bin/bash

echo "========================================"
echo "  MQTT Broker (Mosquitto)"
echo "========================================"
echo ""

# Check if mosquitto is installed
if ! command -v mosquitto &> /dev/null; then
    echo "ERROR: Mosquitto not installed"
    echo "Install: sudo apt install mosquitto mosquitto-clients"
    exit 1
fi

# Stop system service if running
sudo systemctl stop mosquitto 2>/dev/null || true

# Kill any existing process
pkill mosquitto 2>/dev/null || true

sleep 1

echo "Starting Mosquitto on port 1883..."
echo "Allowing anonymous connections"
echo ""
echo "To subscribe to messages, run in another terminal:"
echo "  mosquitto_sub -h localhost -t 'sensors/#' -v"
echo ""
echo "Press Ctrl+C to stop"
echo "========================================"
echo ""

# Start in foreground with verbose logging
mosquitto -v -p 1883
