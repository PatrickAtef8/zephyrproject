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
