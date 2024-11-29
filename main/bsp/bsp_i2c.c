#include "bsp_i2c.h"

#define I2C_MASTER_NUM I2C_NUM_0     // I2C port number


static i2c_cmd_handle_t cmd;

bool bsp_i2c_is_ready(void)
{
  return (cmd == NULL) ? false : true;  // Directly return the result of the condition
}

bool bsp_i2c_is_device_ready(uint16_t dev_addr) 
{
  cmd = i2c_cmd_link_create();
  if (cmd == NULL) 
      return false; 

  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
  i2c_master_stop(cmd);

  esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);

  return (ret == ESP_OK);
}

bool bsp_i2c_write_mem(uint16_t dev_addr, uint16_t mem_addr, uint16_t data)
{
  cmd = i2c_cmd_link_create();  // Create a new I2C command link
  if (cmd == NULL) 
    return false;  // Return false if command link creation failed

  // Start the I2C transmission
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, dev_addr, true); // Send device address with WRITE mode
  i2c_master_write_byte(cmd, mem_addr, true);  // Control byte: Co = 0, D/C# = 0
  i2c_master_write_byte(cmd, data, true);  // Send the actual command
  i2c_master_stop(cmd);  // End the I2C transmission

  // Execute the I2C command
  esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);

  // Delete the command link to free memory
  i2c_cmd_link_delete(cmd);

  // Return true if transaction was successful, false otherwise
  return (ret == ESP_OK);
}









#define SH1106_I2C_ADDRESS 0x3C
#define I2C_PORT I2C_NUM_0 // Change based on your setup

esp_err_t bsp_i2c_write_data(uint16_t dev_addr, uint16_t mem_addr,  uint8_t data, size_t length)
{
    if (data == NULL || length == 0) 
      return ESP_ERR_INVALID_ARG;

    // Create an I2C command link
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (cmd == NULL) 
      return ESP_ERR_NO_MEM;

    i2c_master_start(cmd);

    i2c_master_write_byte(cmd, dev_addr, true);
    i2c_master_write_byte(cmd, mem_addr, true);
    i2c_master_write(cmd, data, length, true);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(1000));

    i2c_cmd_link_delete(cmd);

    return ret;
}
                      
