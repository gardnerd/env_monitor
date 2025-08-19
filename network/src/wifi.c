#include "wifi.h"

#include <string.h>

#include "pico/cyw43_arch.h"

// Max WPA2 SSID and Password lengths (+1 to include null termination)
static char wifi_ssid[MAX_WIFI_SSID_LENGTH + 1];
static char wifi_pw[MAX_WIFI_PW_LENGTH + 1];

bool wifi_init()
{
    if (!cyw43_is_initialized(&cyw43_state))
    {
        // Initialise the Wi-Fi chip
        if (cyw43_arch_init())
        {
            printf("Wi-Fi init failed\n");
            return false;
        }
    }

    cyw43_arch_enable_sta_mode();

    return true;
}

bool wifi_connect(const char *ssid, const char *pw)
{
    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, pw, CYW43_AUTH_WPA2_AES_PSK, 5000))
    {
        printf("failed to connect.\n");
        return false;
    }

    printf("Connected to %s.\n", ssid);
    // Read the ip address in a human readable way
    uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
    printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);

    // If successfully connected, save the creds
    wifi_save_creds(ssid, pw);

    return true;
}

bool wifi_connected()
{
    cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP;
}

void wifi_save_creds(const char *ssid, const char *pw)
{
    strncpy(wifi_ssid, ssid, sizeof(wifi_ssid));
    strncpy(wifi_pw, pw, sizeof(wifi_pw));
}

void wifi_reconnect()
{
    wifi_connect(wifi_ssid, wifi_pw);
}