#include <stdbool.h>

#include "bme68x.h"

void sensing_init(void);
bool sensor_gather(struct bme68x_data *data);