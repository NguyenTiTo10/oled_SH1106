// File: main/sh1106.h

#ifndef SH1106_H
#define SH1106_H

#include <stdint.h>
#include "esp_err.h"

// I2C configuration
#define I2C_MASTER_SCL_IO 22         // GPIO for SCL
#define I2C_MASTER_SDA_IO 21         // GPIO for SDA
#define I2C_MASTER_NUM I2C_NUM_0     // I2C port number
#define I2C_MASTER_FREQ_HZ 400000    // I2C clock frequency
#define OLED_I2C_ADDR 0x3C           // SH1106 I2C address

// Function prototypes
esp_err_t i2c_master_init(void);
esp_err_t sh1106_send_command(uint8_t command);
esp_err_t sh1106_send_data(uint8_t data);
void sh1106_init(void);
void sh1106_clear(void);
void sh1106_draw_char(uint8_t x, uint8_t y, char c);
void sh1106_draw_string(uint8_t x, uint8_t y, const char *str);
void sh1106_set_cursor(uint8_t x, uint8_t y);
void sh1106_fill(uint8_t pattern);

#endif // SH1106_H
