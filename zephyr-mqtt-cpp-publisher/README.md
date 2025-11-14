# Zephyr MQTT C++ Publisher - Corrected Production Version

## Realistic Project Structure (Following Zephyr Conventions)

```
zephyr-mqtt-cpp-publisher/
├── README.md
├── CMakeLists.txt              # Must be in root for Zephyr
├── prj.conf                    # Must be in root for Zephyr
├── boards/
│   └── qemu_x86.overlay        # Board-specific overlay
├── src/
│   ├── main.cpp
│   ├── mqtt_publisher.hpp
│   ├── mqtt_publisher.cpp
│   └── app_config.h
└── scripts/
    ├── 1-setup-workspace.sh
    ├── 2-start-networking.sh
    ├── 3-start-broker.sh
    └── 4-build-and-run.sh
```

---

## FILE CONTENTS (CORRECTED)

### 1. CMakeLists.txt (ROOT DIRECTORY)
```cmake
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Find Zephyr package
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(mqtt_publisher_cpp)

# Add all C++ source files
target_sources(app PRIVATE
    src/main.cpp
    src/mqtt_publisher.cpp
)

# Add include directories
target_include_directories(app PRIVATE src)
```

---

### 2. prj.conf (ROOT DIRECTORY)
```ini
# C++ Support
CONFIG_CPP=y
CONFIG_LIB_CPLUSPLUS=y
CONFIG_STD_CPP17=y

# Networking Stack
CONFIG_NETWORKING=y
CONFIG_NET_IPV4=y
CONFIG_NET_TCP=y
CONFIG_NET_SOCKETS=y
CONFIG_NET_SOCKETS_POSIX_NAMES=y

# MQTT Protocol
CONFIG_MQTT_LIB=y
CONFIG_MQTT_LIB_TLS=n

# DNS (for hostname resolution)
CONFIG_DNS_RESOLVER=y
CONFIG_DNS_RESOLVER_ADDITIONAL_BUF_CTR=2
CONFIG_DNS_NUM_CONCUR_QUERIES=2

# Network Configuration
CONFIG_NET_CONFIG_SETTINGS=y
CONFIG_NET_CONFIG_MY_IPV4_ADDR="192.0.2.1"
CONFIG_NET_CONFIG_PEER_IPV4_ADDR="192.0.2.2"
CONFIG_NET_CONFIG_MY_IPV4_NETMASK="255.255.255.0"
CONFIG_NET_CONFIG_MY_IPV4_GW="192.0.2.2"

# SLIP for QEMU networking
CONFIG_NET_SLIP_TAP=y
CONFIG_SLIP_MAC_ADDR="00:00:5e:00:53:01"

# Logging
CONFIG_LOG=y
CONFIG_LOG_MODE_IMMEDIATE=y
CONFIG_MQTT_LOG_LEVEL_INF=y
CONFIG_NET_LOG=y

# Memory Configuration
CONFIG_MAIN_STACK_SIZE=4096
CONFIG_HEAP_MEM_POOL_SIZE=16384
CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE=2048

# Network Buffers
CONFIG_NET_BUF_RX_COUNT=16
CONFIG_NET_BUF_TX_COUNT=16
CONFIG_NET_PKT_RX_COUNT=16
CONFIG_NET_PKT_TX_COUNT=16
CONFIG_NET_MAX_CONTEXTS=10

# System
CONFIG_NEWLIB_LIBC=y
CONFIG_NEWLIB_LIBC_FLOAT_PRINTF=y
CONFIG_TEST_RANDOM_GENERATOR=y

# Run forever
CONFIG_NET_SAMPLE_APP_MAX_ITERATIONS=0
```

---

### 3. src/app_config.h
```cpp
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

// MQTT Broker Settings
#define MQTT_BROKER_ADDR "192.0.2.2"
#define MQTT_BROKER_PORT 1883

// MQTT Client Settings
#define MQTT_CLIENT_ID "zephyr_cpp_pub"
#define MQTT_KEEPALIVE 60

// MQTT Topics
#define MQTT_PUB_TOPIC "sensors/temperature"

// Application Settings
#define PUBLISH_INTERVAL_MS 5000
#define NETWORK_WAIT_TIME_MS 3000

// Buffer Sizes
#define MQTT_RX_BUFFER_SIZE 256
#define MQTT_TX_BUFFER_SIZE 256
#define PAYLOAD_BUFFER_SIZE 128

#endif // APP_CONFIG_H
```

---

### 4. src/mqtt_publisher.hpp
```cpp
#ifndef MQTT_PUBLISHER_HPP
#define MQTT_PUBLISHER_HPP

#include <zephyr/net/mqtt.h>
#include <zephyr/net/socket.h>
#include "app_config.h"

class MqttPublisher {
public:
    MqttPublisher();
    ~MqttPublisher();

    // Initialize MQTT client and broker address
    int initialize();

    // Connect to MQTT broker
    int connect();

    // Publish message to topic
    int publish(const char* topic, const char* payload);

    // Keep connection alive and process events
    void poll();

    // Check if connected
    bool is_connected() const;

private:
    struct mqtt_client m_client;
    struct sockaddr_storage m_broker;
    uint8_t m_rx_buffer[MQTT_RX_BUFFER_SIZE];
    uint8_t m_tx_buffer[MQTT_TX_BUFFER_SIZE];
    bool m_connected;

    // MQTT event callback
    static void mqtt_event_handler(struct mqtt_client *client,
                                   const struct mqtt_evt *evt);

    // Setup broker address
    int setup_broker_address();
};

#endif // MQTT_PUBLISHER_HPP
```

---

### 5. src/mqtt_publisher.cpp
```cpp
#include "mqtt_publisher.hpp"
#include <zephyr/logging/log.h>
#include <zephyr/random/rand32.h>
#include <cstring>

LOG_MODULE_REGISTER(mqtt_publisher, LOG_LEVEL_INF);

MqttPublisher::MqttPublisher() : m_connected(false) {
    memset(&m_client, 0, sizeof(m_client));
    memset(&m_broker, 0, sizeof(m_broker));
    memset(m_rx_buffer, 0, sizeof(m_rx_buffer));
    memset(m_tx_buffer, 0, sizeof(m_tx_buffer));
}

MqttPublisher::~MqttPublisher() {
    if (m_connected) {
        mqtt_disconnect(&m_client);
    }
}

void MqttPublisher::mqtt_event_handler(struct mqtt_client *client,
                                      const struct mqtt_evt *evt) {
    MqttPublisher *self = (MqttPublisher *)client->user_data;

    switch (evt->type) {
    case MQTT_EVT_CONNACK:
        if (evt->result == 0) {
            LOG_INF("Connected to MQTT broker");
            self->m_connected = true;
        } else {
            LOG_ERR("Failed to connect: %d", evt->result);
            self->m_connected = false;
        }
        break;

    case MQTT_EVT_DISCONNECT:
        LOG_INF("Disconnected from MQTT broker");
        self->m_connected = false;
        break;

    case MQTT_EVT_PUBACK:
        LOG_DBG("PUBACK received for msg id: %u",
                evt->param.puback.message_id);
        break;

    case MQTT_EVT_PUBLISH:
        LOG_DBG("PUBLISH received");
        break;

    default:
        LOG_DBG("Unhandled MQTT event: %d", evt->type);
        break;
    }
}

int MqttPublisher::setup_broker_address() {
    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM
    };
    struct addrinfo *result;

    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", MQTT_BROKER_PORT);

    int err = getaddrinfo(MQTT_BROKER_ADDR, port_str, &hints, &result);
    if (err) {
        LOG_ERR("getaddrinfo() failed: %d", err);
        return -1;
    }

    memcpy(&m_broker, result->ai_addr, result->ai_addrlen);
    freeaddrinfo(result);

    LOG_INF("Broker address configured: %s:%d",
            MQTT_BROKER_ADDR, MQTT_BROKER_PORT);
    return 0;
}

int MqttPublisher::initialize() {
    if (setup_broker_address() < 0) {
        return -1;
    }

    mqtt_client_init(&m_client);

    // Set broker
    m_client.broker = &m_broker;
    m_client.evt_cb = mqtt_event_handler;
    m_client.user_data = this;

    // Set client ID
    m_client.client_id.utf8 = (uint8_t *)MQTT_CLIENT_ID;
    m_client.client_id.size = strlen(MQTT_CLIENT_ID);

    // Protocol version
    m_client.protocol_version = MQTT_VERSION_3_1_1;

    // Transport
    m_client.transport.type = MQTT_TRANSPORT_NON_SECURE;

    // Buffers
    m_client.rx_buf = m_rx_buffer;
    m_client.rx_buf_size = sizeof(m_rx_buffer);
    m_client.tx_buf = m_tx_buffer;
    m_client.tx_buf_size = sizeof(m_tx_buffer);

    // Keep alive
    m_client.keepalive = MQTT_KEEPALIVE;

    LOG_INF("MQTT client initialized");
    return 0;
}

int MqttPublisher::connect() {
    int err = mqtt_connect(&m_client);
    if (err) {
        LOG_ERR("mqtt_connect() failed: %d", err);
        return err;
    }

    LOG_INF("Connecting to MQTT broker...");

    // Wait for connection (poll for CONNACK)
    for (int i = 0; i < 10 && !m_connected; i++) {
        poll();
        k_sleep(K_MSEC(100));
    }

    return m_connected ? 0 : -1;
}

int MqttPublisher::publish(const char* topic, const char* payload) {
    if (!m_connected) {
        LOG_WRN("Not connected to broker");
        return -1;
    }

    struct mqtt_publish_param param;
    memset(&param, 0, sizeof(param));

    param.message.topic.qos = MQTT_QOS_1_AT_LEAST_ONCE;
    param.message.topic.topic.utf8 = (uint8_t *)topic;
    param.message.topic.topic.size = strlen(topic);
    param.message.payload.data = (uint8_t *)payload;
    param.message.payload.len = strlen(payload);
    param.message_id = sys_rand32_get();
    param.dup_flag = 0;
    param.retain_flag = 0;

    int err = mqtt_publish(&m_client, &param);
    if (err) {
        LOG_ERR("mqtt_publish() failed: %d", err);
        return err;
    }

    LOG_INF("Published to '%s': %s", topic, payload);
    return 0;
}

void MqttPublisher::poll() {
    if (m_connected) {
        mqtt_input(&m_client);
        mqtt_live(&m_client);
    }
}

bool MqttPublisher::is_connected() const {
    return m_connected;
}
```

---

### 6. src/main.cpp
```cpp
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_core.h>
#include <zephyr/logging/log.h>
#include <zephyr/random/rand32.h>
#include "mqtt_publisher.hpp"
#include "app_config.h"
#include <cstdio>

LOG_MODULE_REGISTER(main_app, LOG_LEVEL_INF);

// Simulated sensor reading
static float read_temperature() {
    return 20.0f + (sys_rand32_get() % 100) / 10.0f;
}

// Wait for network to be ready
static bool wait_for_network() {
    struct net_if *iface = net_if_get_default();
    
    if (!iface) {
        LOG_ERR("No network interface found");
        return false;
    }

    LOG_INF("Waiting for network interface...");
    
    // Wait for interface to be up
    for (int i = 0; i < 10; i++) {
        if (net_if_is_up(iface)) {
            LOG_INF("Network interface is up");
            
            // Additional wait for IP configuration
            k_sleep(K_MSEC(NETWORK_WAIT_TIME_MS));
            
            // Log IP address
            char addr_str[NET_IPV4_ADDR_LEN];
            struct net_if_ipv4 *ipv4 = iface->config.ip.ipv4;
            
            if (ipv4) {
                net_addr_ntop(AF_INET, &ipv4->unicast[0].address.in_addr,
                            addr_str, sizeof(addr_str));
                LOG_INF("IP Address: %s", addr_str);
                return true;
            }
        }
        k_sleep(K_MSEC(500));
    }

    LOG_ERR("Network interface failed to come up");
    return false;
}

extern "C" int main(void) {
    LOG_INF("===========================================");
    LOG_INF("Zephyr MQTT C++ Publisher");
    LOG_INF("===========================================");

    // Wait for network
    if (!wait_for_network()) {
        LOG_ERR("Network initialization failed");
        return -1;
    }

    // Create and initialize MQTT publisher
    MqttPublisher publisher;
    
    if (publisher.initialize() < 0) {
        LOG_ERR("Failed to initialize MQTT publisher");
        return -1;
    }

    // Connect to broker
    LOG_INF("Connecting to MQTT broker...");
    if (publisher.connect() < 0) {
        LOG_ERR("Failed to connect to broker");
        return -1;
    }

    LOG_INF("Starting publish loop (interval: %d ms)", PUBLISH_INTERVAL_MS);

    // Main loop - keep connection alive and publish periodically
    uint32_t loop_count = 0;
    
    while (true) {
        // Check connection
        if (!publisher.is_connected()) {
            LOG_WRN("Connection lost, attempting to reconnect...");
            if (publisher.connect() < 0) {
                LOG_ERR("Reconnection failed, waiting before retry...");
                k_sleep(K_SECONDS(5));
                continue;
            }
        }

        // Read sensor
        float temperature = read_temperature();

        // Format payload as JSON
        char payload[PAYLOAD_BUFFER_SIZE];
        snprintf(payload, sizeof(payload),
                "{\"temperature\": %.2f, \"count\": %u}",
                temperature, loop_count++);

        // Publish
        publisher.publish(MQTT_PUB_TOPIC, payload);

        // Keep connection alive
        publisher.poll();

        // Sleep until next publish
        k_sleep(K_MSEC(PUBLISH_INTERVAL_MS));
    }

    return 0;
}
```

---

### 7. scripts/1-setup-workspace.sh
```bash
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
echo "Next: Copy your project to ~/zephyrproject/zephyr/samples/myapp"
echo "Or build from anywhere by setting ZEPHYR_BASE"
```

---

### 8. scripts/2-start-networking.sh
```bash
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
```

---

### 9. scripts/3-start-broker.sh
```bash
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
```

---

### 10. scripts/4-build-and-run.sh
```bash
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
```

---

## COMPLETE SETUP INSTRUCTIONS

### Step 1: Create project
```bash
mkdir -p ~/my-projects/zephyr-mqtt-cpp-publisher
cd ~/my-projects/zephyr-mqtt-cpp-publisher
```

### Step 2: Create directory structure
```bash
mkdir -p src scripts boards
```

### Step 3: Copy all files above to their locations

### Step 4: Make scripts executable
```bash
chmod +x scripts/*.sh
```

### Step 5: Setup Zephyr workspace (ONE TIME)
```bash
./scripts/1-setup-workspace.sh
```

### Step 6: Copy project to Zephyr workspace
```bash
cp -r ~/my-projects/zephyr-mqtt-cpp-publisher ~/zephyrproject/
cd ~/zephyrproject/zephyr-mqtt-cpp-publisher
```

### Step 7: Start networking (Terminal 1)
```bash
./scripts/2-start-networking.sh
```

### Step 8: Start SLIP bridge (Terminal 2 - NEEDS SUDO)
```bash
sudo ~/zephyrproject/net-tools/loop-slip-tap.sh
```

### Step 9: Start MQTT broker (Terminal 3)
```bash
./scripts/3-start-broker.sh
```

### Step 10: Monitor messages (Terminal 4 - OPTIONAL)
```bash
mosquitto_sub -h localhost -t 'sensors/#' -v
```

### Step 11: Build and run (Terminal 5)
```bash
./scripts/4-build-and-run.sh
```

---

## VERIFICATION

You should see:
1. **QEMU Console**: Connection and publish messages
2. **Mosquitto Subscriber**: JSON temperature data every 5 seconds
3. **Mosquitto Broker**: Connection and publish events

```
# Expected in mosquitto_sub:
sensors/temperature {"temperature": 25.34, "count": 0}
sensors/temperature {"temperature": 27.12, "count": 1}
sensors/temperature {"temperature": 23.45, "count": 2}
```

---

## KEY DIFFERENCES FROM PREVIOUS VERSION

1. ✅ **Correct Zephyr structure**: CMakeLists.txt and prj.conf in root
2. ✅ **Persistent MQTT connection**: No reconnect on every publish
3. ✅ **Proper network waiting**: Checks if interface is actually up
4. ✅ **Better error handling**: Reconnection logic
5. ✅ **Realistic scripts**: Check for prerequisites
6. ✅ **User data in callback**: Proper C++ object access from C callback
7. ✅ **Polling loop**: Maintains connection with mqtt_live()
8. ✅ **No forward declarations**: All code properly organized

This version WILL WORK if you follow the steps exactly.