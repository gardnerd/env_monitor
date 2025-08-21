#include <stdbool.h>
#include <stdint.h>

#include "lwip/ip.h"

#define MAX_WIFI_SSID_LENGTH (32)
#define MAX_WIFI_PW_LENGTH (63)

typedef struct
{
    char wifi_ssid[MAX_WIFI_SSID_LENGTH + 1];
    char wifi_pw[MAX_WIFI_PW_LENGTH + 1];
    ip_addr_t mqtt_ip;
    uint16_t mqtt_port;
} config_t;

void config_reset(void);
config_t *config_get(void);
void config_set(config_t *config);
