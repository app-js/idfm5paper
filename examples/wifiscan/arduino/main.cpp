#include "M5EPD.h"
#include "WiFi.h"

#define MAX_SCAN 16

M5EPD_Canvas canvas(&M5.EPD);

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
    
    log_d("idfm5: begin");
    M5.begin();
    log_d("idfm5: Clear");
    M5.EPD.Clear(true);
    log_d("idfm5: SetRotation");
    M5.EPD.SetRotation(90);
    log_d("idfm5: createCanvas");
    canvas.createCanvas(540,960);
    log_d("idfm5: setTextSize");
    canvas.setTextSize(4);
    log_d("idfm5: drawString");
    canvas.drawString("Wifi scan...", 10, 10);
    log_d("idfm5: pushCanvas");
    canvas.pushCanvas(0,0,UPDATE_MODE_DU4);

    // WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    bool async = true;
    int wifi_num = 0;
    Serial.println("scanNetworks");
    wifi_num = WiFi.scanNetworks(async);

    if (!async && wifi_num<0)
    {
       Serial.print("err scanNetworks: "); 
       Serial.println(wifi_num);
    }
    else
    {
        log_d("idfm5: WiFi.scanComplete");
        int tests = 0;
        while(1 && async)
        {
            tests++;
            wifi_num = WiFi.scanComplete();
            if(wifi_num >= 0)
            {
                break;
            }
            delay(500);
        }

        char buf[100];
        canvas.setTextSize(3);
        int fh = 50;
        sprintf(buf,"Total APs scanned: %u", wifi_num);
        canvas.drawString(buf, 10, 50);

        if (wifi_num == 0) 
        {
            Serial.println("no networks found");
        } 
        else 
        {

            if (async)
            {
                Serial.print(tests*500);
                Serial.println("ms scanCompletes done");
            }
            Serial.print(wifi_num);
            Serial.println(" networks found");
            for (int i = 0; i < wifi_num; ++i) 
            {
                if (i<=MAX_SCAN)
                {
                    sprintf(buf,"%d: %s", i, WiFi.SSID(i).c_str());
                    canvas.drawString(buf, 10, 90+i*fh);
                }

                Serial.print(i + 1);
                Serial.print(": ");
                Serial.print(WiFi.SSID(i));
                Serial.print(" (");
                Serial.print(WiFi.RSSI(i));
                Serial.print(")");
                Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
                delay(10);
            }
        }
    }

    log_d("idfm5: pushCanvas");
    canvas.pushCanvas(0,0,UPDATE_MODE_DU4);
}

void loop()
{
}

void loopTask(void *pvParameters)
{
    setup();
    for(;;) {
        micros(); //update overflow
        loop();
    }
}

extern "C" void app_main()
{
    initArduino();
    xTaskCreatePinnedToCore(loopTask, "loopTask", 8192, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
}
