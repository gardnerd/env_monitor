#include <stdio.h>
#include "pico/stdlib.h"

#include "webserver.h"
#include "access_point.h"
#include "wifi.h"
#include "io.h"
#include "sensing.h"

// temporary until flash storage
#include "secrets/wifi_creds.h"

int main()
{
    stdio_init_all();

    io_init();
    sensing_init();

    // Enable ap if not configured
    // Otherwise enable wifi

    // Initialize Network
    if (access_point_init())
    {
        webserver_init();
    }

    wifi_save_creds(WIFI_SSID, WIFI_PW);

    while (true)
    {
        if (io_reset_active())
        {
            sensor_gather();
        }

        // if (!wifi_connected())
        // {
        //     wifi_reconnect();
        // }
    }
}
