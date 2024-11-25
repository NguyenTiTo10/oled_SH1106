#include "bsp_timer.h"

void bsp_delay (uint32_t time)
{
  vTaskDelay(time / portTICK_PERIOD_MS);
}
