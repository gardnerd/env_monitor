#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

#include "webserver.h"
#include "access_point.h"
#include "wifi.h"
#include "io.h"
#include "sensing.h"
#include "config.h"

// temporary until flash storage
#include "secrets/wifi_creds.h"

int main()
{
    stdio_init_all();

    io_init();
    sensing_init();

    // wifi_save_creds(WIFI_SSID, WIFI_PW);

    if (io_reset_active())
    {
        while (io_reset_active())
            sleep_ms(100);
        printf("reset configuration\n");
        config_reset();
    }

    // Initialize Network
    //
    // Enable Wifi if configured
    // Otherwise enable AP
    char *wifi_ssid = config_get()->wifi_ssid;
    printf("config [0x%02x]\n", wifi_ssid[0]);

    // If the flash is set
    bool wifi_configured = wifi_ssid[0] != 0xff;
    if (wifi_configured)
    {
        printf("Wifi configured with SSID: %s\n", wifi_ssid);
        if (!wifi_init())
        {
            printf("Wifi failed to init");
            return -1;
        }
    }
    else
    {
        if (!access_point_init())
        {
            printf("AP failed to init\n");
            return -1;
        }
    }

    webserver_init();

    while (true)
    {
        if (wifi_configured && !wifi_connected())
        {
            wifi_reconnect();
        }
    }
}
