// File: main/sh1106.h

#ifndef SH1106_H
#define SH1106_H

#include <stdint.h>
#include "esp_err.h"

// Function prototypes
esp_err_t i2c_master_init(void);
esp_err_t drv_sh1106_send_command(uint8_t command);
esp_err_t drv_sh1106_write_data(uint8_t data);
void drv_sh1106_init(void);
void sh1106_clear(void);
void sh1106_draw_char(uint8_t x, uint8_t y, char c);
void sh1106_draw_string(uint8_t x, uint8_t y, const char *str);
void sh1106_set_cursor(uint8_t x, uint8_t y);
void sh1106_fill(uint8_t pattern);

#endif // SH1106_H
