#include <stdbool.h>

void io_init(void);
void reboot(void);
bool io_reset_active(void);
void io_set_wifi_led(bool state);