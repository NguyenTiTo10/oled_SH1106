idf_component_register(SRCS "main.c"
                       "bsp/bsp_i2c.c"
                       "bsp/bsp_timer.c"
                       "driver/drv_sh1106.c"
                       INCLUDE_DIRS "." "bsp" "driver" "middleware")
