#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifndef BSP_TIMER_H
#define BSP_TIMER_H

void bsp_delay (uint32_t time);

#endif