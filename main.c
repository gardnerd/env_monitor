#include <stdio.h>
#include <string.h>

#include "pico/async_context.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"

#include "webserver.h"
#include "access_point.h"
#include "wifi.h"
#include "mqtt_client.h"
#include "io.h"
#include "sensing.h"
#include "config.h"

#define WATCHDOG_TIMEOUT_MS (30 * 1000)

#define SENSOR_WORKER_PERIOD_MS (10 * 1000)
static void sensor_async_worker_fn(async_context_t *context, struct async_work_on_timeout *timeout);
static async_at_time_worker_t sensor_worker = {
    .do_work = sensor_async_worker_fn};

#define WIFI_WORKER_PERIOD_MS (5 * 1000)
static void wifi_async_worker_fn(async_context_t *context, struct async_work_on_timeout *timeout);
static async_at_time_worker_t wifi_worker = {
    .do_work = wifi_async_worker_fn};

int main()
{
    stdio_init_all();

    watchdog_enable(WATCHDOG_TIMEOUT_MS, true);

    io_init();
    sensing_init();

    if (io_reset_active())
    {
        while (io_reset_active())
            sleep_ms(100);
        printf("reset configuration\n");
        config_reset();
    }

    char *wifi_ssid = config_get()->wifi_ssid;
    printf("config [0x%02x]\n", wifi_ssid[0]);

    // Check if the wifi is configured
    //      Flash erases to 0xFF, so that will be an unconfigured state
    bool wifi_configured = wifi_ssid[0] != 0xff;
    if (wifi_configured)
    {
        printf("Wifi configured with SSID: %s\n", wifi_ssid);
        if (!wifi_init())
        {
            printf("Wifi failed to init");
            return -1;
        }

        if (!mqtt_client_init())
        {
            printf("MQTT failed to init\n");
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

    // The wifi_init or access_point_init calls will setup the async_context
    async_context_t *context = cyw43_arch_async_context();
    if (wifi_configured)
    {
        async_context_add_at_time_worker(context, &wifi_worker);
        async_context_add_at_time_worker(context, &sensor_worker);
    }

    while (true)
    {
        watchdog_update();
        async_context_wait_for_work_ms(context, 5000);
    }
}

static void wifi_async_worker_fn(async_context_t *context, struct async_work_on_timeout *timeout)
{
    if (wifi_reconnect() && mqtt_reconnect())
    {

        io_set_wifi_led(true);
    }
    else
    {
        io_set_wifi_led(false);
    }

    async_context_add_at_time_worker_in_ms(context, &wifi_worker, WIFI_WORKER_PERIOD_MS);
}

static void sensor_async_worker_fn(async_context_t *context, struct async_work_on_timeout *timeout)
{
    struct bme68x_data data;
    if (sensor_gather(&data))
    {
        char mqtt_payload[100] = {0};
        sprintf(mqtt_payload, "{\"temperature\": %.2f, \"pressure\": %.2f, \"humidity\": %.2f}", data.temperature, data.pressure / 1000, data.humidity);
        if (mqtt_client_publish("living_room/sensor1", mqtt_payload, strlen(mqtt_payload)))
        {
            printf("%.2f C, %.2f kPa, %.2f%% RH\n",
                   data.temperature,
                   data.pressure / 1000,
                   data.humidity);
        }
    }
    async_context_add_at_time_worker_in_ms(context, &sensor_worker, SENSOR_WORKER_PERIOD_MS);
}
