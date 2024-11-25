#include "bsp_i2c.h"

#define I2C_MASTER_NUM I2C_NUM_0     // I2C port number


// static i2c_cmd_handle_t cmd;

// bool bsp_i2c_link_create(void)
// {
//   return (cmd = i2c_cmd_link_create()) ? true : false;
// }

// bool bsp_i2c_is_ready(void)
// {
//   return (cmd == NULL) ? false : true;  // Directly return the result of the condition
// }

// bool bsp_i2c_master_write(uint8_t data)
// {
//   if (!bsp_i2c_is_ready())
//     return false;
    
//   return (i2c_master_write_byte(cmd, data, true) == ESP_OK);
// }

// bool bsp_i2c_stop_transmit(void)
// {
//   if (!bsp_i2c_is_ready())
//     return false;
//   return (i2c_master_stop(cmd)) ? true : false;
// }

// bool bsp_i2c_delete_link(void)
// {
//   if (cmd != NULL) 
//   {
//     i2c_cmd_link_delete(cmd);  // Delete the command link
//     cmd = NULL;                // Clear the pointer to avoid dangling references
//   }
//   return true;              // Return true, as the deletion task is logically complete
// }

bool bsp_i2c_send_command(uint16_t dev_addr, uint8_t command)
{
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();  // Create a new I2C command link
  if (cmd == NULL) 
    return false;  // Return false if command link creation failed

  // Start the I2C transmission
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true); // Send device address with WRITE mode
  i2c_master_write_byte(cmd, 0x00, true);  // Control byte: Co = 0, D/C# = 0
  i2c_master_write_byte(cmd, command, true);  // Send the actual command
  i2c_master_stop(cmd);  // End the I2C transmission

  // Execute the I2C command
  esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);

  // Delete the command link to free memory
  i2c_cmd_link_delete(cmd);

  // Return true if transaction was successful, false otherwise
  return (ret == ESP_OK);
}






// static esp_err_t drv_sh1106_write_data(uint8_t data)
// {
//     i2c_cmd_handle_t cmd = i2c_cmd_link_create();       // Create I2C command link
//     i2c_master_start(cmd);                              // Start I2C transmission
//     i2c_master_write_byte(cmd, (OLED_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
//                                                         // OLED address in write mode
//     i2c_master_write_byte(cmd, 0x40, true);             // Control byte: Co = 0, D/C# = 1 (data mode)
//     i2c_master_write_byte(cmd, data, true);             // Send the data byte
//     i2c_master_stop(cmd);                               // Stop I2C transmission
//     esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
//                                                         // Execute the I2C transaction
//     i2c_cmd_link_delete(cmd);                           // Delete the command link
//     return ret;                                         // Return status
// }
