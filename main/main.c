// File: main/test_sh1106.c

#include <stdio.h>
#include "driver/i2c.h"
#include "esp_err.h"

#include "bsp_timer.h"
#include "bsp_i2c.h"

#include "drv_sh1106.h"

#include "image_128x64.h"

// I2C configuration
#define I2C_MASTER_SCL_IO       22              
#define I2C_MASTER_SDA_IO       21              
#define I2C_MASTER_NUM          I2C_NUM_0       
#define I2C_MASTER_FREQ_HZ      400000          

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
    i2c_param_config(I2C_MASTER_NUM, &conf);                                      
    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);          
}


static void app_test_oled_sh1106 (void)
{
     // Display Image
    drv_sh1106_clear_screen();
    drv_sh1106_display_image(image_logo_uit);
    bsp_delay (3000);
    drv_sh1106_clear_screen();

    // Display text 
    drv_sh1106_write_string(0, 0, "Hello, ESP-IDF!");
    drv_sh1106_write_string(0, 1, "Multiline OK!");
    drv_sh1106_write_string(0, 2, "ANOTHER LINE 2");
    drv_sh1106_write_string(0, 3, "ANOTHER LINE 3");
    drv_sh1106_write_string(0, 4, "ANOTHER LINE 4");
    drv_sh1106_write_string(0, 5, "ANOTHER LINE 5");
    drv_sh1106_write_string(0, 6, "ANOTHER LINE 6");
    drv_sh1106_write_string(0, 7, "ANOTHER LINE 7");

    bsp_delay (3000);
    drv_sh1106_clear_screen();


    // Display image
    drv_sh1106_display_image(image_logo_ce);
    bsp_delay (3000);
    drv_sh1106_clear_screen();
    drv_sh1106_turn_off();
}

void app_main(void) 
{
    // Initialize I2C and the SH1106 OLED display
    if (i2c_master_init() == ESP_OK) 
        printf("I2C initialized successfully.\n");
    else 
        printf("Failed to initialize I2C.\n");

    if (drv_sh1106_init() == ESP_OK)
        printf("Oled SH1106 initialized successfully.\n");

    
    while (1)
    {
        app_test_oled_sh1106();
    }
    
}
