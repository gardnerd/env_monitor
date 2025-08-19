#include "access_point.h"

#include "pico/cyw43_arch.h"

#include "dhcpserver.h"

static char ap_ssid[] = "test_network";
static char ap_pw[] = "password";

static dhcp_server_t dhcp_server = {0};

ip_addr_t ap_ip;
ip_addr_t ap_mask;

bool access_point_init()
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

    cyw43_arch_enable_ap_mode(ap_ssid, ap_pw, CYW43_AUTH_WPA2_AES_PSK);

    ip4_addr_set_u32(&ap_ip, PP_HTONL(CYW43_DEFAULT_IP_AP_ADDRESS));
    ip4_addr_set_u32(&ap_mask, PP_HTONL(CYW43_DEFAULT_IP_MASK));
    dhcp_server_init(&dhcp_server, &ap_ip, &ap_mask);

    return true;
}