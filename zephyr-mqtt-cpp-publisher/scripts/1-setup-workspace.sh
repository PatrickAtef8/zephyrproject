#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "========================================"
echo "  Zephyr Workspace Setup"
echo "========================================"
echo ""
echo "This will set up Zephyr in: ~/zephyrproject"
echo ""

# Check if already exists
if [ -d "$HOME/zephyrproject/zephyr" ]; then
    echo "Zephyr workspace already exists at ~/zephyrproject"
    read -p "Do you want to update it? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        cd ~/zephyrproject
        west update
        echo "Workspace updated"
    fi
    exit 0
fi

# Install dependencies
echo "Installing dependencies..."
sudo apt update
sudo apt install -y --no-install-recommends git cmake ninja-build gperf \
  ccache dfu-util device-tree-compiler wget \
  python3-dev python3-pip python3-setuptools python3-tk python3-wheel \
  xz-utils file make gcc gcc-multilib g++-multilib libsdl2-dev \
  libmagic1 socat libpcap-dev mosquitto mosquitto-clients

# Install West
echo "Installing West..."
pip3 install --user -U west

# Make sure it's in PATH
export PATH="$HOME/.local/bin:$PATH"

# Initialize workspace
echo "Initializing Zephyr workspace..."
cd ~
west init zephyrproject
cd zephyrproject

# Update
echo "Updating Zephyr (this may take a while)..."
west update

# Export
west zephyr-export

# Install Python requirements
pip3 install --user -r zephyr/scripts/requirements.txt

# Clone net-tools
echo "Setting up net-tools..."
git clone https://github.com/zephyrproject-rtos/net-tools.git
cd net-tools
make

echo ""
echo "========================================"
echo "  Setup Complete!"
echo "========================================"
echo ""
echo "Next: Copy your project to ~/zephyrproject/"
echo "  cp -r $PROJECT_ROOT ~/zephyrproject/"
