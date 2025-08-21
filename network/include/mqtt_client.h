#include <stdint.h>
#include <stdbool.h>

#include "lwip/ip.h"

void mqtt_save_creds(ip_addr_t mqtt_ip, uint16_t mqtt_port);
bool mqtt_client_init(void);
bool mqtt_reconnect(void);
bool mqtt_client_publish(const char *topic, const uint8_t *payload, uint16_t payload_len);