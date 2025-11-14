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
