// File: main/sh1106.h

#ifndef DRV_SH1106_H
#define DRV_SH1106_H

#include <stdint.h>


void drv_sh1106_init(void);
void drv_sh1106_clear_screen(void);
void drv_sh1106_write_string(uint8_t x, uint8_t y, const char *str);
void drv_sh1106_display_image(const uint8_t *image);


// void sh1106_set_cursor(uint8_t x, uint8_t y);
// void drv_sh1106_fill_screen(uint8_t pattern);
  

#endif // SH1106_H
