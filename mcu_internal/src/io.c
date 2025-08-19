#include "io.h"

#include <stdio.h>

#include "pico/bootrom.h"
#include "boot/picoboot_constants.h"
#include "hardware/gpio.h"

#define RESET_GPIO (16)

void io_init()
{
    gpio_init(RESET_GPIO);
    gpio_set_dir(RESET_GPIO, false);
    gpio_pull_up(RESET_GPIO);
}

void reboot()
{
    printf("reboot\n");
    rom_reboot(REBOOT2_FLAG_REBOOT_TYPE_NORMAL | REBOOT2_FLAG_NO_RETURN_ON_SUCCESS, 1, 0, 0);
}

bool io_reset_active()
{
    return !gpio_get(RESET_GPIO);
}
