// File: main/sh1106_driver.c

#include <stdio.h>
#include "driver/i2c.h"
#include "drv_sh1106.h"
#include "font8x8_basic.h"

// I2C configuration
#define I2C_MASTER_SCL_IO 22         // GPIO for SCL
#define I2C_MASTER_SDA_IO 21         // GPIO for SDA
#define I2C_MASTER_NUM I2C_NUM_0     // I2C port number
#define I2C_MASTER_FREQ_HZ 400000    // I2C clock frequency
#define OLED_I2C_ADDR 0x3C           // SH1106 I2C address

esp_err_t i2c_master_init(void) 
{
    i2c_config_t conf = 
    {
        .mode = I2C_MODE_MASTER,      
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);                                // Apply the configuration         
    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);          // Install the I2C driver
}


/**
 * @brief
 * Purpose:
 *      Sending command to SH1106    
 * Key Details:
 *      OLED_I2C_ADDR << 1: Shifts the OLED address left to add the read/write bit. 
 *      The I2C protocol combines address and mode into one byte.
 *      0x00: Control byte for command transmission. The D/C# bit determines whether the next byte is a command (0) or data (1).
 *      i2c_master_cmd_begin: Executes the queued I2C operations within a 1-second timeout.     
 */
esp_err_t sh1106_send_command(uint8_t command) 
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();       // Creates a new I2C command link
    i2c_master_start(cmd);                              // Starts an I2C transmission
    i2c_master_write_byte(cmd, (OLED_I2C_ADDR << 1) | I2C_MASTER_WRITE, true); 
                                                        // Sends OLED address with WRITE mode
    i2c_master_write_byte(cmd, 0x00, true);             // Control byte: Co = 0 (single byte), D/C# = 0 (command mode)
    i2c_master_write_byte(cmd, command, true);          // Send the actual command
    i2c_master_stop(cmd);                               // Ends the I2C transmission
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS); 
                                                        // Executes the I2C transaction
    i2c_cmd_link_delete(cmd);                           // Deletes the I2C command link
    return ret;                                         // Returns the status of the transaction
}


esp_err_t sh1106_send_data(uint8_t data) 
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (OLED_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x40, true); // Co = 0, D/C# = 1 for data
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

void sh1106_init(void) 
{
    uint8_t init_cmds[] = 
    {
        0xAE,       // Display OFF
        0xD5, 0x80, // Set display clock divide ratio/oscillator frequency
        0xA8, 0x3F, // Set multiplex ratio to 1/64 duty
        0xD3, 0x00, // Set display offset
        0x40,       // Set display start line to 0
        0x8D, 0x14, // Enable charge pump
        0xA1,       // Set segment re-map (normal)
        0xC8,       // Set COM output scan direction (remapped)
        0xDA, 0x12, // Set COM pins hardware configuration
        0x81, 0x7F, // Set contrast control
        0xD9, 0xF1, // Set pre-charge period
        0xDB, 0x20, // Set VCOMH deselect level
        0xA4,       // Set entire display ON (resume to RAM content)
        0xA6,       // Set normal display (non-inverted)
        0xAF        // Display ON
    };
    for (int i = 0; i < sizeof(init_cmds); i++) {
        sh1106_send_command(init_cmds[i]);
    }
}

void sh1106_clear(void) 
{
    for (uint8_t page = 0; page < 8; page++) {
        sh1106_send_command(0xB0 + page); // Set page address
        sh1106_send_command(0x00); // Set lower column address
        sh1106_send_command(0x10); // Set higher column address
        for (uint8_t col = 0; col < 132; col++) {
            sh1106_send_data(0x00); // Clear column data
        }
    }
}

void sh1106_draw_char(uint8_t x, uint8_t y, char c) 
{
    if (x >= 128 || y >= 8) {
        return; // Prevent out-of-bounds drawing
    }

    sh1106_send_command(0xB0 + y);          // Set page address
    sh1106_send_command(0x00 + (x & 0x0F)); // Set lower column address
    sh1106_send_command(0x10 + (x >> 4));   // Set higher column address

    for (int i = 0; i < 8; i++) {
        sh1106_send_data(font8x8_basic_tr[(uint8_t)c][i]);
    }
}

void sh1106_draw_string(uint8_t x, uint8_t y, const char *str) 
{
    while (*str) 
    {
        sh1106_draw_char(x, y, *str++);
        x += 8; // Move to next character position
        if (x >= 128) { // Wrap to the next line if necessary
            x = 0;
            y++;
            if (y >= 8) {
                break; // Stop if out of vertical bounds
            }
        }
    }
}

void sh1106_fill(uint8_t pattern) 
{
    for (uint8_t page = 0; page < 8; page++) 
    {
        sh1106_send_command(0xB0 + page); // Set page address
        sh1106_send_command(0x00); // Set lower column address
        sh1106_send_command(0x10); // Set higher column address
        for (uint8_t col = 0; col < 132; col++) {
            sh1106_send_data(pattern); // Fill column with pattern
        }
    }
}
