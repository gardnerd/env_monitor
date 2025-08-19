#include "config.h"

#include <stdio.h>

#include "hardware/flash.h"

// Only using 1 core so this should be plenty far away
#define FLASH_DATA_OFFSET (2 * 1024 * 1024)

const uint8_t *flash_data_contents = (const uint8_t *)(XIP_BASE + FLASH_DATA_OFFSET);

void config_reset()
{
    flash_range_erase(FLASH_DATA_OFFSET, FLASH_SECTOR_SIZE);
}

config_t *config_get()
{
    return (config_t *)flash_data_contents;
}

void config_set(config_t *config)
{
    flash_range_program(FLASH_DATA_OFFSET, (uint8_t *)config, sizeof(config_t));
}
