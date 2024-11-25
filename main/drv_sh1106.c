// File: main/sh1106_driver.c

#include "drv_sh1106.h"
#include "font8x8_basic.h"


#define I2C_MASTER_NUM  I2C_NUM_0       // I2C port number
#define OLED_I2C_ADDR   0x3C            // SH1106 I2C address
#define COMMAND_MODE    0x00            // Control byte: Co = 0 (single byte), D/C# = 0 
                                        // Send command to oled
#define DATA_MODE       0x40            // Control byte: Co = 0, D/C# = 1 (data mode)
                                        // Represent pixel data to display

// OLED resolution macros
#define OLED_WIDTH 132          // OLED width in pixels
#define OLED_HEIGHT 64           // OLED height in pixels

static uint8_t screen_buffer[OLED_WIDTH * (OLED_HEIGHT / 8)];   // Frame buffer

static esp_err_t drv_sh1106_send_command(uint8_t command);
static esp_err_t drv_sh1106_write_data(uint8_t data);
static esp_err_t drv_sh1106_draw_pixel(uint8_t x, uint8_t y, uint8_t color);
static esp_err_t drv_sh1106_update_screen(void);


// #define VERSION_1
#ifdef VERSION_1
static esp_err_t drv_sh1106_send_command(uint8_t command) 
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

static esp_err_t drv_sh1106_write_data(uint8_t data) 
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();       // Create I2C command link
    i2c_master_start(cmd);                              // Start I2C transmission
    i2c_master_write_byte(cmd, (OLED_I2C_ADDR << 1) | I2C_MASTER_WRITE, true); 
                                                        // OLED address in write mode
    i2c_master_write_byte(cmd, 0x40, true);             // Control byte: Co = 0, D/C# = 1 (data mode)
    i2c_master_write_byte(cmd, data, true);             // Send the data byte
    i2c_master_stop(cmd);                               // Stop I2C transmission
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS); 
                                                        // Execute the I2C transaction
    i2c_cmd_link_delete(cmd);                           // Delete the command link
    return ret;                                         // Return status
}
#else
static esp_err_t drv_sh1106_send_command(uint8_t command) 
{
    bool ret = false;                                     
    ret = bsp_i2c_write_mem((OLED_I2C_ADDR << 1) | I2C_MASTER_WRITE, COMMAND_MODE, command);
    return (ret == true) ? ESP_OK : ESP_FAIL;
}

static esp_err_t drv_sh1106_write_data(uint8_t data)
{
  bool ret = false;                                     
  ret = bsp_i2c_write_mem((OLED_I2C_ADDR << 1) | I2C_MASTER_WRITE, DATA_MODE, data);
  return (ret == true) ? ESP_OK : ESP_FAIL;
}  
#endif

void drv_sh1106_init(void) 
{
    if (!bsp_i2c_is_device_ready(OLED_I2C_ADDR))
        return;

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
        drv_sh1106_send_command(init_cmds[i]);
    }
}

void drv_sh1106_clear_screen(void) 
{
    for (uint8_t page = 0; page < (OLED_HEIGHT / 8); page++) 
    {
        drv_sh1106_send_command(0xB0 + page); // Set page address
        drv_sh1106_send_command(0x00); // Set lower column address
        drv_sh1106_send_command(0x10); // Set higher column address
        for (uint8_t col = 0; col < OLED_WIDTH; col++) {
            drv_sh1106_write_data(0x00); // Clear column data
        }
    }
}

static void drv_sh1106_write_char(uint8_t x, uint8_t y, char c) 
{
    if (x >= OLED_WIDTH || y >= (OLED_HEIGHT / 8)) 
        return; // Prevent out-of-bounds drawing

    uint8_t adjusted_x = x + 2; // Adjust by 2 to account for the SH1106 column offset

    drv_sh1106_send_command(0xB0 + y);            // Set page address
    drv_sh1106_send_command(0x00 + (adjusted_x & 0x0F)); // Set lower column address
    drv_sh1106_send_command(0x10 + (adjusted_x >> 4));   // Set higher column address

    for (int i = 0; i < (OLED_HEIGHT / 8); i++) 
    {
        drv_sh1106_write_data(font8x8_basic_tr[(uint8_t)c][i]);
    }
}


void drv_sh1106_write_string(uint8_t x, uint8_t y, const char *str) 
{
    uint8_t start_x = x + 2; // Adjust start position for SH1106 offset

    while (*str) 
    {
        drv_sh1106_write_char(start_x, y, *str++);
        start_x += 8; // Move to the next character position
        if (start_x >= OLED_WIDTH) 
        { // Wrap to the next line if necessary
            start_x = 2; // Reset to adjusted start
            y++;
            if (y >= (OLED_HEIGHT / 8)) 
                break; // Stop if out of vertical bounds
        }
    }
}

// #define DISPLAY_IMAGE_VERSION_1

#ifdef DISPLAY_IMAGE_VERSION_1
void drv_sh1106_display_image(const uint8_t *image) 
{
    for (uint8_t page = 0; page < (OLED_HEIGHT / 8); page++) 
    {
        drv_sh1106_send_command(0xB0 + page); // Set page address
        drv_sh1106_send_command(0x00);       // Set lower column address
        drv_sh1106_send_command(0x10);       // Set higher column address
        
        for (uint8_t col = 0; col < OLED_WIDTH; col++) 
        {
            // Send image data for the current page and column
            drv_sh1106_write_data(image[page * OLED_WIDTH + col]);
        }
    }
}

#else
static esp_err_t drv_sh1106_draw_pixel(uint8_t x, uint8_t y, uint8_t color)
{
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT)
        return ESP_ERR_INVALID_ARG; // Out of bounds

    uint16_t index = x + (y / 8) * OLED_WIDTH; // Calculate buffer index
    uint8_t bit = 1 << (y % 8);                // Determine the bit within the byte

    if (color)
        screen_buffer[index] |= bit;  // Set pixel (turn ON)
    else
        screen_buffer[index] &= ~bit; // Clear pixel (turn OFF)

    return ESP_OK;
}

static esp_err_t drv_sh1106_update_screen(void)
{
    for (uint8_t page = 0; page < (OLED_HEIGHT / 8); page++)
    {
        drv_sh1106_send_command(0xB0 + page); // Set page address
        drv_sh1106_send_command(0x00);        // Set lower column address
        drv_sh1106_send_command(0x10);        // Set higher column address

        for (uint8_t col = 0; col < OLED_WIDTH; col++)
        {
            drv_sh1106_write_data(screen_buffer[page * OLED_WIDTH + col]);
        }
    }
    return ESP_OK;
}

void drv_sh1106_display_image(const uint8_t *image)
{
    for (uint8_t y = 0; y < OLED_HEIGHT; y++)
    {
        for (uint8_t x = 0; x < OLED_WIDTH; x++)
        {
            uint16_t byte_index = x + (y / 8) * OLED_WIDTH;
            uint8_t bit = (image[byte_index] >> (y % 8)) & 0x01; // Extract the bit
            drv_sh1106_draw_pixel(x, y, bit);                   // Draw the pixel
        }
    }
    drv_sh1106_update_screen(); // Send the updated buffer to the display
}

#endif