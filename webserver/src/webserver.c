#include "webserver.h"

#include "pico/cyw43_arch.h"
#include "lwip/apps/httpd.h"

void init_webserver()
{
    cyw43_arch_lwip_begin();
    httpd_init();
    cyw43_arch_lwip_end();
}

err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                       u16_t http_request_len, int content_len, char *response_uri,
                       u16_t response_uri_len, u8_t *post_auto_wnd)
{
    return ERR_OK;
}

err_t httpd_post_receive_data(void *connection, struct pbuf *p)
{
    printf("total_len %d\n", p->tot_len);

    pbuf_free(p);
    return ERR_OK;
}

void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len)
{
    snprintf(response_uri, response_uri_len, "/wifi_cred_ok.shtml");
}