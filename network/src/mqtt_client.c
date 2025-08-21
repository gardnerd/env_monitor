#include "mqtt_client.h"

#include "pico/cyw43_arch.h"
#include "lwip/apps/mqtt.h"

#include "config.h"

static mqtt_client_t *mqtt_client;
static char mqtt_clientid[50] = {0};
static struct mqtt_connect_client_info_t mqtt_client_info = {0};
static bool connecting = false;
static bool publish_pending = false;

static void mqtt_connection_callback(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
static void mqtt_publish_callback(void *arg, err_t err);

void mqtt_save_creds(ip_addr_t mqtt_ip, uint16_t mqtt_port)
{
    config_t config = {0};
    memcpy(&config, config_get(), sizeof(config_t));

    ip_addr_copy(config.mqtt_ip, mqtt_ip);
    config.mqtt_port = mqtt_port;

    config_set(&config);
}

bool mqtt_client_init()
{
    mqtt_client = mqtt_client_new();
    if (mqtt_client == NULL)
    {
        printf("MQTT client init failed\n");
        return false;
    }

    // Use the mac for the clientId
    uint8_t mac[6];
    cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA, mac);
    sprintf(mqtt_clientid, "envmon_%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    mqtt_client_info.client_id = mqtt_clientid;
    mqtt_client_info.keep_alive = 10;

    return true;
}

bool mqtt_reconnect()
{
    // if mqtt not connected connect
    if (mqtt_client_is_connected(mqtt_client))
    {
        return true;
    }
    else
    {
        if (connecting)
        {
            return false;
        }

        printf("Connect MQTT\n");
        connecting = true;
        mqtt_client_connect(mqtt_client, &(config_get()->mqtt_ip), config_get()->mqtt_port, mqtt_connection_callback, &connecting, &mqtt_client_info);
    }

    return false;
}

bool mqtt_client_publish(const char *topic, const uint8_t *payload, uint16_t payload_len)
{
    // serialize publishing to keep things simple
    if (!mqtt_client_is_connected(mqtt_client) || publish_pending)
    {
        return false;
    }

    publish_pending = true;
    return mqtt_publish(mqtt_client, topic, payload, payload_len, 0, 0, mqtt_publish_callback, &publish_pending) == ERR_OK;
}

void mqtt_connection_callback(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    bool *connecting = (bool *)arg;
    *connecting = false;
}

void mqtt_publish_callback(void *arg, err_t err)
{
    bool *publish_pending = (bool *)arg;
    *publish_pending = false;
}