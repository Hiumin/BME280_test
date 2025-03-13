#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "bme280.h"

extern void pms7003_task(void *pvParameters);
extern esp_err_t bme280_check_connection();
extern void read_bme280_data(void);

void app_main(void)
{
    xTaskCreate(pms7003_task, "pms7003_task", 4096, NULL, 5, NULL);
    bme280_check_connection();
    read_bme280_data();
}
