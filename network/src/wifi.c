#include "wifi.h"

#include <string.h>

#include "pico/cyw43_arch.h"
#include "config.h"

static bool wifi_connect(const char *ssid, const char *pw);
static int prev_link_status = CYW43_LINK_DOWN;

bool wifi_init()
{
    if (!cyw43_is_initialized(&cyw43_state))
    {
        // Initialise the Wi-Fi chip
        if (cyw43_arch_init_with_country(CYW43_COUNTRY_USA))
        {
            printf("Wi-Fi init failed\n");
            return false;
        }
    }

    cyw43_arch_enable_sta_mode();

    return true;
}

void wifi_save_creds(const char *ssid, const char *pw)
{
    config_t config = {0};
    memcpy(&config, config_get(), sizeof(config_t));

    strncpy(config.wifi_ssid, ssid, sizeof(config.wifi_ssid));
    strncpy(config.wifi_pw, pw, sizeof(config.wifi_pw));

    config_set(&config);
}

bool wifi_reconnect()
{
    int current_link_status = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
    if (current_link_status != CYW43_LINK_UP)
    {
        prev_link_status = current_link_status;
        printf("Connecting to Wi-Fi...\n");
        int ret = cyw43_arch_wifi_connect_async(config_get()->wifi_ssid, config_get()->wifi_pw, CYW43_AUTH_WPA2_AES_PSK);
        if (ret != 0)
        {
            printf("failed to connect. ERROR: %d\n", ret);
        }
    }
    else if (prev_link_status != CYW43_LINK_UP)
    {

        printf("Connected to %s.\n", config_get()->wifi_ssid);

        uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
        printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);

        prev_link_status = current_link_status;
    }

    return current_link_status == CYW43_LINK_UP;
}