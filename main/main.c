// File: main/test_sh1106.c

#include <stdio.h>
#include "drv_sh1106.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"
#include "esp_err.h"
#include "image_128x64.h"

// I2C configuration
#define I2C_MASTER_SCL_IO 22         // GPIO for SCL
#define I2C_MASTER_SDA_IO 21         // GPIO for SDA
#define I2C_MASTER_NUM I2C_NUM_0     // I2C port number
#define I2C_MASTER_FREQ_HZ 400000    // I2C clock frequency

static esp_err_t i2c_master_init(void);

static esp_err_t i2c_master_init(void) 
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

void app_main(void) 
{
    // Initialize I2C and the SH1106 OLED display
    if (i2c_master_init() == ESP_OK) 
        printf("I2C initialized successfully.\n");
    else 
        printf("Failed to initialize I2C.\n");

    drv_sh1106_init();
    printf("SH1106 initialized successfully.\n");

    drv_sh1106_clear_screen();
    vTaskDelay(500 / portTICK_PERIOD_MS);

#ifdef DISPLAY_TEXT
    // // Test displaying a string on the OLED
    // drv_sh1106_write_string(0, 0, "Hello, ESP-IDF!");
    // drv_sh1106_write_string(0, 1, "Multiline OK!");
    // drv_sh1106_write_string(0, 5, "Another line.!");

    // vTaskDelay(3000 / portTICK_PERIOD_MS);

    // // Fill the display with a pattern
    // drv_sh1106_fill_screen(0xAA); // Example pattern
    // vTaskDelay(1000 / portTICK_PERIOD_MS);

#else
    drv_sh1106_display_image(logo_uit_image_updated);
#endif


}
