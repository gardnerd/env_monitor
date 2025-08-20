#include <stdbool.h>

bool wifi_init(void);
void wifi_save_creds(const char *ssid, const char *pw);
bool wifi_reconnect(void);
