/* m5paper template
*/

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "Arduino.h"
#include <M5EPD.h>

extern "C" void app_main()
{
    initArduino();
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);
}
