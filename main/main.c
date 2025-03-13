#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern void pms7003_task(void *pvParameters);

void app_main(void)
{
    xTaskCreate(pms7003_task, "pms7003_task", 4096, NULL, 5, NULL);
}
