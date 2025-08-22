#include "webserver.h"

#include "pico/cyw43_arch.h"
#include "lwip/apps/httpd.h"
#include "lwip/ip.h"
#include "wifi.h"
#include "mqtt_client.h"
#include "config.h"

static bool get_http_param(struct pbuf *p, const char *param_name, char *param_value, size_t param_value_max_len);

void webserver_init()
{
    cyw43_arch_lwip_begin();
    httpd_init();
    cyw43_arch_lwip_end();
}

err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                       u16_t http_request_len, int content_len, char *response_uri,
                       u16_t response_uri_len, u8_t *post_auto_wnd)
{
    snprintf(response_uri, response_uri_len, "/setup_err.shtml");
    return ERR_OK;
}

err_t httpd_post_receive_data(void *connection, struct pbuf *p)
{
    printf("total_len %d\n", p->tot_len);

    char wifi_ssid[MAX_WIFI_SSID_LENGTH + 1] = {0};

    if (!get_http_param(p, "ssid=", wifi_ssid, sizeof(wifi_ssid)))
    {
        pbuf_free(p);
        return ERR_VAL;
    }

    char wifi_pw[MAX_WIFI_PW_LENGTH + 1] = {0};
    if (!get_http_param(p, "pw=", wifi_pw, sizeof(wifi_pw)))
    {
        pbuf_free(p);
        return ERR_VAL;
    }

    char mqtt_addr_str[IP4ADDR_STRLEN_MAX] = {0};
    if (!get_http_param(p, "mqtt_ip=", mqtt_addr_str, sizeof(mqtt_addr_str)))
    {
        pbuf_free(p);
        return ERR_VAL;
    }

    char mqtt_port_str[6] = {0};
    if (!get_http_param(p, "mqtt_port=", mqtt_port_str, sizeof(mqtt_port_str)))
    {
        pbuf_free(p);
        return ERR_VAL;
    }

    ip_addr_t mqtt_addr;
    ip4addr_aton(mqtt_addr_str, &mqtt_addr);
    uint16_t mqtt_port = atoi(mqtt_port_str);

    // printf("ssid = \"%s\", pw = \"%s\"\n", wifi_ssid, wifi_pw);
    // printf("mqtt_ip = \"%s\", mqtt_port = \"%s\"\n", mqtt_addr_str, mqtt_port_str);

    wifi_save_creds(wifi_ssid, wifi_pw);
    mqtt_save_creds(mqtt_addr, mqtt_port);

    pbuf_free(p);
    return ERR_OK;
}

void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len)
{
    snprintf(response_uri, response_uri_len, "/setup_ok.shtml");
}

bool get_http_param(struct pbuf *p, const char *param_name, char *param_value, size_t param_value_max_len)
{
    size_t param_name_len = strlen(param_name);
    u16_t param_name_start_pos = pbuf_memfind(p, param_name, param_name_len, 0);
    if (param_name_start_pos == 0xFFFF)
    {
        // Failed to find the param
        return false;
    }

    u16_t param_value_start_pos = param_name_start_pos + param_name_len;

    // Either delimited by & or it's the end of the buf
    u16_t param_value_end_pos = pbuf_memfind(p, "&", 1, param_name_start_pos);
    if (param_value_end_pos == 0xFFFF)
    {
        param_value_end_pos = p->tot_len;
    }

    u16_t param_value_len = param_value_end_pos - param_value_start_pos;

    char *buf = pbuf_get_contiguous(p, param_value, param_value_max_len, param_value_len, param_value_start_pos);
    if (buf == NULL)
    {
        return false;
    }

    // Most of the time the contents is just going to be fully in the pbuf. Copy it anyway if that's the case.
    if (buf != param_value)
    {
        strncpy(param_value, buf, param_value_len);
    }

    return true;
}