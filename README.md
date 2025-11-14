# Zephyr MQTT C++ Publisher

[![Zephyr](https://img.shields.io/badge/Zephyr-RTOS-blue)](https://zephyrproject.org/)
[![C++](https://img.shields.io/badge/C++-17-orange)](https://en.cppreference.com/)
[![MQTT](https://img.shields.io/badge/MQTT-3.1.1-green)](https://mqtt.org/)
[![QEMU](https://img.shields.io/badge/QEMU-Emulated-red)](https://www.qemu.org/)

> A modern C++ MQTT publisher application built on Zephyr RTOS, demonstrating IoT sensor telemetry in an emulated environment using QEMU.

## Quick Start

```bash
# Clone the repository
git clone https://github.com/PatrickAtef8/zephyr-mqtt-cpp-publisher.git
cd zephyr-mqtt-cpp-publisher

# Setup Zephyr workspace (first time only)
./scripts/1-setup-workspace.sh

# Copy project to Zephyr workspace
cp -r . ~/zephyrproject/zephyr-mqtt-cpp-publisher
cd ~/zephyrproject/zephyr-mqtt-cpp-publisher

# Terminal 1: Start networking
./scripts/2-start-networking.sh

# Terminal 2: Start SLIP bridge (needs sudo)
sudo ~/zephyrproject/net-tools/loop-slip-tap.sh

# Terminal 3: Start MQTT broker
./scripts/3-start-broker.sh

# Terminal 4: Build and run
./scripts/4-build-and-run.sh
```

**Within 5 minutes**, you'll have a simulated IoT device publishing temperature data to an MQTT broker!

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Configuration](#configuration)
- [Development](#development)
- [Testing](#testing)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)
- [Resources](#resources)

---

## Overview

This project demonstrates a **production-ready MQTT publisher** for IoT applications using:

- **Zephyr RTOS**: Lightweight, scalable real-time OS for embedded systems
- **Modern C++17**: Classes, RAII, lambdas, and type safety in resource-constrained environments
- **MQTT Protocol**: Publish-subscribe messaging for efficient IoT communication
- **QEMU Emulation**: Hardware-free development and testing on x86 architecture

### Why This Project?

- **Learn by Doing**: Hands-on experience with embedded networking and RTOS concepts
- **No Hardware Required**: Fully functional in QEMU emulation
- **Modern Practices**: Demonstrates C++ best practices in embedded systems
- **Extensible**: Easy to adapt for real hardware (ESP32, nRF52, STM32, etc.)
- **IoT Ready**: Foundation for building production IoT telemetry systems

### Use Cases

- Sensor data telemetry (temperature, humidity, pressure)
- Industrial monitoring systems
- Smart home device communication
- Learning embedded systems and RTOS development
- Prototyping IoT applications before hardware availability

---

## Features

### Core Functionality
- **MQTT Publisher**: Persistent connection to broker with automatic reconnection
- **Simulated Sensor**: Random temperature generation (easily replaceable with real sensors)
- **Automatic Reconnection**: Handles network disruptions gracefully
- **QoS Level 1**: At-least-once delivery guarantee
- **Extensible Security**: Ready for TLS/SSL integration

### Technical Highlights
- **Modern C++17**: Object-oriented design with RAII principles
- **Comprehensive Logging**: Debug-friendly output with Zephyr logging subsystem
- **Low Memory Footprint**: Optimized for resource-constrained devices
- **Configurable**: Easy customization via Kconfig and header files
- **QEMU Ready**: Virtual networking with SLIP/TAP interface

---

## Architecture

```
                              HOST SYSTEM
    ┌───────────────────────────────────────────────────────┐
    │                                                       │
    │   ┌─────────────────┐          ┌──────────────────┐   │
    │   │  MQTT Broker    │          │ MQTT Subscriber  │   │
    │   │  (Mosquitto)    │          │ (mosquitto_sub)  │   │
    │   │  Port: 1883     │          │                  │   │
    │   └────────┬────────┘          └────────┬─────────┘   │
    │            │                            │             │
    │            └────────────┬───────────────┘             │
    │                         │                             │
    │                  ┌──────▼──────┐                      │
    │                  │ TAP Bridge  │                      │
    │                  │ 192.0.2.2   │                      │
    │                  └──────┬──────┘                      │
    │                         │                             │
    │                         │ SLIP Protocol               │
    │                         │                             │
    │         ┌───────────────▼────────────────┐            │
    │         │     QEMU x86 Emulation         │            │
    │         │                                │            │
    │         │  ┌──────────────────────────┐  │            │
    │         │  │   Zephyr RTOS            │  │            │
    │         │  │   IP: 192.0.2.1          │  │            │
    │         │  │                          │  │            │
    │         │  │  ┌──────────────────┐    │  │            │
    │         │  │  │ Application Layer│    │  │            │
    │         │  │  │                  │    │  │            │
    │         │  │  │  MqttPublisher   │    │  │            │
    │         │  │  │  (C++ Class)     │    │  │            │
    │         │  │  │                  │    │  │            │
    │         │  │  │  ┌──────────┐    │    │  │            │
    │         │  │  │  │  Sensor  │    │    │  │            │
    │         │  │  │  │ Simulator│    │    │  │            │
    │         │  │  │  └─────┬────┘    │    │  │            │
    │         │  │  │        │         │    │  │            │
    │         │  │  │        ▼         │    │  │            │
    │         │  │  │  ┌──────────┐    │    │  │            │
    │         │  │  │  │  MQTT    │    │    │  │            │
    │         │  │  │  │  Client  │    │    │  │            │
    │         │  │  │  │ (QoS 1)  │    │    │  │            │
    │         │  │  │  └─────┬────┘    │    │  │            │
    │         │  │  └────────┼─────────┘    │  │            │
    │         │  │           │              │  │            │
    │         │  │  ┌────────▼─────────┐    │  │            │
    │         │  │  │ Network Stack    │    │  │            │
    │         │  │  │ (TCP/IP)         │    │  │            │
    │         │  │  └──────────────────┘    │  │            │
    │         │  └──────────────────────────┘  │            │
    │         └────────────────────────────────┘            │
    │                                                       │
    └───────────────────────────────────────────────────────┘
```

**Key Components:**
1. **Sensor Layer**: Generates telemetry data (simulated or real hardware)
2. **MQTT Client**: Manages persistent connections, publishes to topics
3. **Network Stack**: Zephyr's TCP/IP implementation over SLIP
4. **QEMU Bridge**: Virtual networking connecting guest to host
5. **Broker**: Mosquitto distributes messages to subscribers

---

## Prerequisites

### System Requirements
- **OS**: Linux (Ubuntu 20.04/22.04 recommended) or macOS
  - Windows users: Use WSL2 or a Linux VM
- **RAM**: 4GB minimum, 8GB recommended
- **Disk**: 2GB free space for Zephyr SDK

### Software Dependencies

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install -y git cmake ninja-build gperf \
  ccache dfu-util device-tree-compiler wget \
  python3-dev python3-pip python3-setuptools python3-tk \
  python3-wheel xz-utils file make gcc gcc-multilib \
  g++-multilib libsdl2-dev libmagic1 socat libpcap-dev \
  mosquitto mosquitto-clients
```

### Zephyr SDK
Will be installed automatically by setup script, or manually:
```bash
# Install West meta-tool
pip3 install --user west

# Verify installation
west --version  # Should show 0.14.0 or higher
```

---

## Installation

### Step 1: Clone Repository
```bash
git clone git@github.com:PatrickAtef8/zephyrproject.git
```

### Step 2: Make Scripts Executable
```bash
chmod +x scripts/*.sh
```

### Step 3: Setup Zephyr Workspace (ONE TIME ONLY)
```bash
./scripts/1-setup-workspace.sh
```

This will:
- Install West if not present
- Initialize Zephyr workspace in `~/zephyrproject`
- Download Zephyr RTOS and dependencies
- Clone and build net-tools for QEMU networking
- Install Python requirements

### Step 4: Copy Project to Zephyr Workspace
```bash
cp -r . ~/zephyrproject/zephyr-mqtt-cpp-publisher
cd ~/zephyrproject/zephyr-mqtt-cpp-publisher
```

### Step 5: Verify Installation
```bash
west build --version
# Should display Zephyr version (e.g., 3.5.0 or later)
```

---

## Usage

### Running the Application

You need **4 terminals** to run the complete system:

#### Terminal 1: Start Virtual Networking (SOCAT)
```bash
cd ~/zephyrproject/zephyr-mqtt-cpp-publisher
./scripts/2-start-networking.sh
```
Keep this running. You should see: `starting socat...`

#### Terminal 2: Start SLIP Bridge (REQUIRES SUDO)
```bash
sudo ~/zephyrproject/net-tools/loop-slip-tap.sh
```
Keep this running. You should see network interface creation messages.

#### Terminal 3: Start MQTT Broker
```bash
cd ~/zephyrproject/zephyr-mqtt-cpp-publisher
./scripts/3-start-broker.sh
```
Keep this running. You should see Mosquitto startup messages.

#### Terminal 4: Build and Run Application
```bash
cd ~/zephyrproject/zephyr-mqtt-cpp-publisher
./scripts/4-build-and-run.sh
```
This will build the project and launch QEMU.

#### Terminal 5 (Optional): Monitor MQTT Messages
```bash
mosquitto_sub -h localhost -t 'sensors/#' -v
```

### Expected Output

**QEMU Console (Terminal 4):**
```
*** Booting Zephyr OS build v3.5.0 ***
[00:00:00.010,000] <inf> main_app: ===========================================
[00:00:00.011,000] <inf> main_app: Zephyr MQTT C++ Publisher
[00:00:00.012,000] <inf> main_app: ===========================================
[00:00:00.013,000] <inf> main_app: Waiting for network interface...
[00:00:00.520,000] <inf> main_app: Network interface is up
[00:00:03.530,000] <inf> main_app: IP Address: 192.0.2.1
[00:00:03.535,000] <inf> mqtt_publisher: Broker address configured: 192.0.2.2:1883
[00:00:03.540,000] <inf> mqtt_publisher: MQTT client initialized
[00:00:03.545,000] <inf> main_app: Connecting to MQTT broker...
[00:00:03.550,000] <inf> mqtt_publisher: Connecting to MQTT broker...
[00:00:04.055,000] <inf> mqtt_publisher: Connected to MQTT broker
[00:00:04.060,000] <inf> main_app: Starting publish loop (interval: 5000 ms)
[00:00:04.065,000] <inf> mqtt_publisher: Published to 'sensors/temperature': {"temperature": 25.34, "count": 0}
[00:00:09.070,000] <inf> mqtt_publisher: Published to 'sensors/temperature': {"temperature": 27.12, "count": 1}
[00:00:14.075,000] <inf> mqtt_publisher: Published to 'sensors/temperature': {"temperature": 23.45, "count": 2}
```

**Subscriber Terminal (Terminal 5):**
```
sensors/temperature {"temperature": 25.34, "count": 0}
sensors/temperature {"temperature": 27.12, "count": 1}
sensors/temperature {"temperature": 23.45, "count": 2}
```

**Mosquitto Broker (Terminal 3):**
```
1699012345: New connection from 192.0.2.1:45678 on port 1883.
1699012345: New client connected from 192.0.2.1:45678 as zephyr_cpp_pub (p2, c1, k60).
1699012345: Received PUBLISH from zephyr_cpp_pub (d0, q1, r0, m1, 'sensors/temperature', ... (40 bytes))
```

### Stop Execution
- **QEMU**: Press `Ctrl+A`, then `X`
- **All other terminals**: Press `Ctrl+C`

---

## Project Structure

```
zephyr-mqtt-cpp-publisher/
├── README.md                   # This file
├── CMakeLists.txt              # Zephyr build configuration
├── prj.conf                    # Zephyr Kconfig options
├── LICENSE                     # MIT License
├── .gitignore                  # Git ignore rules
│
├── boards/                     # Board-specific overlays
│   └── qemu_x86.overlay        # QEMU x86 configuration (optional)
│
├── src/                        # Application source code
│   ├── main.cpp                # Entry point with main loop
│   ├── mqtt_publisher.hpp      # MqttPublisher class interface
│   ├── mqtt_publisher.cpp      # MqttPublisher implementation
│   └── app_config.h            # Application constants
│
└── scripts/                    # Automation scripts
    ├── 1-setup-workspace.sh    # One-time Zephyr workspace setup
    ├── 2-start-networking.sh   # Start SOCAT for SLIP
    ├── 3-start-broker.sh       # Launch Mosquitto broker
    └── 4-build-and-run.sh      # Build and execute in QEMU
```

---

## Configuration

### Network Settings
Edit `src/app_config.h`:
```cpp
#define MQTT_BROKER_ADDR "192.0.2.2"  // MQTT broker IP
#define MQTT_BROKER_PORT 1883          // Standard MQTT port
#define MQTT_CLIENT_ID "zephyr_cpp_pub" // Unique client ID
```

### MQTT Topic
```cpp
#define MQTT_PUB_TOPIC "sensors/temperature"  // Publish destination
```

### Publishing Interval
```cpp
#define PUBLISH_INTERVAL_MS 5000  // Publish every 5 seconds
```

### Zephyr Options
Edit `prj.conf`:
```ini
# Enable C++ support
CONFIG_CPP=y
CONFIG_LIB_CPLUSPLUS=y
CONFIG_STD_CPP17=y

# Networking
CONFIG_NETWORKING=y
CONFIG_NET_IPV4=y
CONFIG_MQTT_LIB=y

# Logging verbosity
CONFIG_LOG=y
CONFIG_LOG_MODE_IMMEDIATE=y

# Memory optimization
CONFIG_MAIN_STACK_SIZE=4096
CONFIG_HEAP_MEM_POOL_SIZE=16384
```

### QoS Levels
Modify in `src/mqtt_publisher.cpp`:
```cpp
param.message.topic.qos = MQTT_QOS_0_AT_MOST_ONCE;   // Fire-and-forget
param.message.topic.qos = MQTT_QOS_1_AT_LEAST_ONCE;  // Acknowledged (default)
param.message.topic.qos = MQTT_QOS_2_EXACTLY_ONCE;   // Two-phase commit
```

---

## Development

### Adding Real Sensors

Replace the simulated sensor in `src/main.cpp`:

```cpp
// Include Zephyr sensor API
#include <zephyr/drivers/sensor.h>

static float read_temperature() {
    const struct device *sensor = DEVICE_DT_GET_ONE(ti_hdc2010);
    struct sensor_value temp;
    
    if (!device_is_ready(sensor)) {
        LOG_ERR("Sensor not ready");
        return -999.0f;
    }
    
    sensor_sample_fetch(sensor);
    sensor_channel_get(sensor, SENSOR_CHAN_AMBIENT_TEMP, &temp);
    
    return sensor_value_to_float(&temp);
}
```

### Building for Real Hardware

**ESP32 Example:**
```bash
west build -b esp32_devkitc_wroom -p auto
west flash
```

**nRF52840 Example:**
```bash
west build -b nrf52840dk_nrf52840 -p auto
west flash --runner jlink
```

**STM32 Example:**
```bash
west build -b nucleo_f429zi -p auto
west flash
```

### Enabling TLS/SSL

**Step 1:** Add to `prj.conf`:
```ini
CONFIG_MQTT_LIB_TLS=y
CONFIG_NET_SOCKETS_SOCKOPT_TLS=y
CONFIG_MBEDTLS=y
CONFIG_MBEDTLS_BUILTIN=y
CONFIG_MBEDTLS_ENABLE_HEAP=y
CONFIG_MBEDTLS_HEAP_SIZE=32768
```

**Step 2:** Modify `src/mqtt_publisher.cpp`:
```cpp
// In initialize():
m_client.transport.type = MQTT_TRANSPORT_SECURE;

// Add TLS configuration
static sec_tag_t sec_tag_list[] = { 1 };
m_client.transport.tls.config.peer_verify = TLS_PEER_VERIFY_REQUIRED;
m_client.transport.tls.config.cipher_list = NULL;
m_client.transport.tls.config.sec_tag_list = sec_tag_list;
m_client.transport.tls.config.sec_tag_count = ARRAY_SIZE(sec_tag_list);
m_client.transport.tls.config.hostname = MQTT_BROKER_ADDR;
```

### Code Style
- **Indentation**: 4 spaces (no tabs)
- **Naming**: 
  - Classes: `PascalCase`
  - Functions: `snake_case`
  - Constants: `UPPER_SNAKE_CASE`
- **Headers**: Use `#pragma once` or include guards
- **Logging**: Use `LOG_INF/WRN/ERR` macros

---

## Testing

### Manual Testing

**1. Network Connectivity**
```bash
# Ping emulated device from host
ping 192.0.2.1

# Check TAP interface
ip addr show tap0
```

**2. MQTT Broker Connectivity**
```bash
# Subscribe to all topics
mosquitto_sub -h localhost -t "#" -v

# Test publish from command line
mosquitto_pub -h localhost -t "test/topic" -m "hello"
```

**3. Message Validation**
```bash
# Subscribe with formatted output
mosquitto_sub -h localhost -t "sensors/temperature" -F "%I %t %p"
```

**4. Stress Testing**
Modify `PUBLISH_INTERVAL_MS` to `1000` (1 second) in `src/app_config.h` and rebuild:
```bash
# Count messages per minute
mosquitto_sub -h localhost -t "sensors/temperature" | pv -l -i 60 > /dev/null
```

---

## Troubleshooting

### Build Errors

**Error: `west: command not found`**
```bash
# Solution: Install West and add to PATH
pip3 install --user west
export PATH="$HOME/.local/bin:$PATH"
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
```

**Error: `CMake Error: Could not find Zephyr`**
```bash
# Solution: Run from correct directory
cd ~/zephyrproject/zephyr-mqtt-cpp-publisher
west build -b qemu_x86 -p auto
```

**Error: `undefined reference to std::__throw_bad_alloc`**
```bash
# Solution: Enable C++ library in prj.conf
echo "CONFIG_LIB_CPLUSPLUS=y" >> prj.conf
west build -b qemu_x86 -p auto
```

### Network Issues

**QEMU can't reach broker**
```bash
# Check TAP interface exists
ip addr show tap0

# If missing, restart SLIP bridge
sudo pkill loop-slip-tap
sudo ~/zephyrproject/net-tools/loop-slip-tap.sh
```

**Error: `Network interface failed to come up`**
```bash
# Solution: Ensure networking scripts are running
# Terminal 1: ./scripts/2-start-networking.sh
# Terminal 2: sudo ~/zephyrproject/net-tools/loop-slip-tap.sh
# Wait 5 seconds, then run application
```

### MQTT Connection Failures

**Error: `Failed to connect to broker`**
```bash
# Check Mosquitto is running
ps aux | grep mosquitto

# Check Mosquitto allows anonymous connections
cat /etc/mosquitto/mosquitto.conf
# Should contain:
listener 1883
allow_anonymous true

# Restart Mosquitto
sudo systemctl restart mosquitto
# Or use script:
./scripts/3-start-broker.sh
```

**Error: `Connection refused`**
```bash
# Verify broker is listening
netstat -tuln | grep 1883
# Or
ss -tuln | grep 1883

# Test broker connectivity
mosquitto_pub -h localhost -t "test" -m "hello"
```

### QEMU Issues

**No console output**
```bash
# Enable immediate logging in prj.conf
echo "CONFIG_LOG_MODE_IMMEDIATE=y" >> prj.conf
west build -b qemu_x86 -p auto
```

**QEMU crashes or won't start**
```bash
# Check QEMU installation
which qemu-system-x86_64

# Install if missing
sudo apt install qemu-system-x86

# Clean build and retry
rm -rf build
west build -b qemu_x86 -p auto
```

**Can't exit QEMU**
```bash
# Press Ctrl+A (release), then X
# Or kill from another terminal:
pkill qemu-system-x86
```

---

## Resources

### Official Documentation
- [Zephyr Project](https://docs.zephyrproject.org/) - RTOS documentation
- [Zephyr MQTT Sample](https://docs.zephyrproject.org/latest/samples/net/mqtt_publisher/README.html) - Official MQTT example
- [MQTT.org](https://mqtt.org/) - Protocol specifications
- [QEMU Documentation](https://www.qemu.org/docs/master/) - Emulator guide

### Tutorials & Guides
- [Getting Started with Zephyr](https://docs.zephyrproject.org/latest/getting_started/)
- [MQTT Essentials](https://www.hivemq.com/mqtt-essentials/) - Comprehensive MQTT guide
- [Modern C++ for Embedded](https://embedded-artistry.com/blog/2018/04/26/embedded-c/) - Best practices

### Community
- [Zephyr Discord](https://discord.gg/zephyr) - Real-time chat
- [Zephyr GitHub](https://github.com/zephyrproject-rtos/zephyr) - Source code & issues
- [MQTT Community](https://mqtt.org/community/) - Protocol discussions

### Related Projects
- [Eclipse Mosquitto](https://mosquitto.org/) - Popular MQTT broker
- [HiveMQ](https://www.hivemq.com/) - Enterprise MQTT platform
- [Node-RED](https://nodered.org/) - Flow-based IoT programming

---

<div align="center">

[Back to Top](#zephyr-mqtt-c-publisher)

</div>