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
