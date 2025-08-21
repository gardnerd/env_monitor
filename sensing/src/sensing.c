#include "sensing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15

static uint8_t dev_addr;
static struct bme68x_dev bme_dev = {0};
static struct bme68x_conf bme_conf = {0};
static struct bme68x_heatr_conf bme_heatr_conf = {0};

static BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
static BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
static void bme68x_delay_us(uint32_t period, void *intf_ptr);

void sensing_init()
{
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    dev_addr = BME68X_I2C_ADDR_HIGH;
    bme_dev.read = bme68x_i2c_read;
    bme_dev.write = bme68x_i2c_write;
    bme_dev.intf = BME68X_I2C_INTF;
    bme_dev.delay_us = bme68x_delay_us;
    bme_dev.intf_ptr = &dev_addr;
    bme_dev.amb_temp = 25;

    if (bme68x_init(&bme_dev) != BME68X_OK)
    {
        printf("sensor failed initialization\n");
    }

    bme_conf.filter = BME68X_FILTER_OFF;
    bme_conf.odr = BME68X_ODR_NONE;
    bme_conf.os_hum = BME68X_OS_16X;
    bme_conf.os_pres = BME68X_OS_1X;
    bme_conf.os_temp = BME68X_OS_2X;
    if (bme68x_set_conf(&bme_conf, &bme_dev) != BME68X_OK)
    {
        printf("sensor failed configuration\n");
    }

    // Don't know how to interpret this reading yet so disable for now
    bme_heatr_conf.enable = BME68X_DISABLE;
    if (bme68x_set_heatr_conf(BME68X_FORCED_MODE, &bme_heatr_conf, &bme_dev) != BME68X_OK)
    {
        printf("sensor failed heater configuration\n");
    }
}

bool sensor_gather(struct bme68x_data *data)
{
    if (bme68x_set_op_mode(BME68X_FORCED_MODE, &bme_dev) != BME68X_OK)
    {
        printf("sensor gather: set op mode failed\n");
        return false;
    }

    /* Calculate delay period in microseconds */
    uint32_t delay_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, &bme_conf, &bme_dev);
    bme_dev.delay_us(delay_period, bme_dev.intf_ptr);

    uint8_t data_count;
    int8_t result = bme68x_get_data(BME68X_FORCED_MODE, data, &data_count, &bme_dev);
    if (result != BME68X_OK)
    {
        printf("sensor gather: get data failed ERROR code (%d)\n", result);
        return false;
    }

    if (data_count == 0)
    {
        printf("sensor gather: no data gathered\n");
        return false;
    }

    return true;
}

BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t device_addr = *(uint8_t *)intf_ptr;

    int bytes_written = i2c_write_blocking(I2C_PORT, device_addr, &reg_addr, 1, true);
    if (bytes_written == PICO_ERROR_GENERIC || bytes_written != 1)
    {
        return -1;
    }

    int bytes_read = i2c_read_blocking(I2C_PORT, device_addr, reg_data, len, false);
    if (bytes_read == PICO_ERROR_GENERIC || bytes_read != len)
    {
        return -1;
    }

    return 0;
}

BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t device_addr = *(uint8_t *)intf_ptr;

    uint32_t concat_len = len + 1;

    // Put the reg_addr and reg_data in the same buffer so it's all in one i2c write (The chip gets upset if it's split up)
    uint8_t *concat_data = calloc(concat_len, sizeof(uint8_t));
    concat_data[0] = reg_addr;
    memcpy(concat_data + 1, reg_data, len);

    int8_t bytes_written = i2c_write_blocking(I2C_PORT, device_addr, concat_data, concat_len, false);
    if (bytes_written == PICO_ERROR_GENERIC || bytes_written != concat_len)
    {
        free(concat_data);
        return -1;
    }

    free(concat_data);
    return 0;
}

void bme68x_delay_us(uint32_t period, void *intf_ptr)
{
    sleep_us(period);
}