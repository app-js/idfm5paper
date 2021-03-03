#include <M5EPD.h>
#include "epdgui/epdgui.h"
#include "frame/frame.h"
#include "systeminit.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "wifi_manager.h"

void info()
{
    log_d("Info:");
    log_d("Internal Total heap %d, internal Free Heap %d", ESP.getHeapSize(), ESP.getFreeHeap());
    log_d("SPIRam Total heap %d, SPIRam Free Heap %d", ESP.getPsramSize(), ESP.getFreePsram()); 
    log_d("ChipRevision %d, Cpu Freq %d, SDK Version %s",ESP.getChipRevision(), ESP.getCpuFreqMHz(), ESP.getSdkVersion());
    log_d("Flash Size %d, Flash Speed %d",ESP.getFlashChipSize(), ESP.getFlashChipSpeed());

    log_d("Test malloc()...");
    uint8_t *testMallocBuffer = (uint8_t*)ps_malloc(900000);
    if(testMallocBuffer != NULL) log_d("NOT NULL"); else log_d("NULL");
    free(testMallocBuffer);

    log_d("Test new()...");
    uint8_t *testNewBuffer = new uint8_t[900000];
    if(testNewBuffer) log_d("NOT NULL"); else log_d("NULL");
    delete testNewBuffer;

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    log_d("This is ESP32 chip with %d CPU cores, %s%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "2.4GHz WiFi, " : "",
            (chip_info.features & CHIP_FEATURE_BT) ? "Bluetooth Classic, " : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "Bluetooth LE" : "");

    log_d("silicon revision %d, ", chip_info.revision);

    log_d("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}


void setup()
{
    info();

    pinMode(M5EPD_MAIN_PWR_PIN, OUTPUT);
    M5.enableMainPower();

    SysInit_Start();
}

void loop()
{
    EPDGUI_MainLoop();
}

void loopTask(void *pvParameters)
{
    setup();
    for(;;) 
    {
        micros(); //update overflow
        loop();
    }
}

extern "C" void app_main()
{
    initArduino();

	/* start the wifi manager */
	wifi_manager_start();

    xTaskCreatePinnedToCore(loopTask, "loopTask", 8192, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
}
