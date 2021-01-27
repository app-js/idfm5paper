#include <M5EPD.h>
#include "epdgui/epdgui.h"
#include "frame/frame.h"
#include "systeminit.h"

void setup()
{
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
