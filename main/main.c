// File: main/test_sh1106.c

#include <stdio.h>
#include "drv_sh1106.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void) {
    // Initialize I2C and the SH1106 OLED display
    if (i2c_master_init() == ESP_OK) {
        printf("I2C initialized successfully.\n");
    } else {
        printf("Failed to initialize I2C.\n");
        return;
    }

    drv_sh1106_init();
    printf("SH1106 initialized successfully.\n");

    // Clear the display
    drv_sh1106_clear_screen();
    vTaskDelay(500 / portTICK_PERIOD_MS);

    // Test displaying a string on the OLED
    drv_sh1106_write_string(0, 0, "Hello, ESP-IDF!");
    drv_sh1106_write_string(0, 1, "Multiline OK!");
    drv_sh1106_write_string(0, 2, "Another line.!");

    vTaskDelay(3000 / portTICK_PERIOD_MS);

    // // Fill the display with a pattern
    // sh1106_fill(0xAA); // Example pattern
    // vTaskDelay(1000 / portTICK_PERIOD_MS);


    // Clear the display again
    drv_sh1106_clear_screen();

    drv_sh1106_write_string(0, 5, "Welcome to Viet Nam");


}
