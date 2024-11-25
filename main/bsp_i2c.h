#ifndef BSP_I2C_H
#define BSP_I2C_H

#include "driver/i2c.h"
#include "esp_err.h"

bool bsp_i2c_master_write(uint16_t dev_addr, uint8_t command);


#endif