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
