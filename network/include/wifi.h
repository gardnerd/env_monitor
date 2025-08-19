#include <stdbool.h>

bool wifi_init(void);
bool wifi_connect(const char *ssid, const char *pw);
void wifi_save_creds(const char *ssid, const char *pw);
bool wifi_connected(void);
void wifi_reconnect(void);
