#include "M5EPD.h"
#include "FS.h"
#include "SPIFFS.h"
#include "binaryttf.h"

M5EPD_Canvas canvas(&M5.EPD);

void setup()
{
    M5.begin();
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);
    M5.RTC.begin();
    log_d("idfm5: createCanvas");
    canvas.createCanvas(540,960);
    log_d("idfm5: setTextSize");
    canvas.setTextSize(3);
    canvas.drawString("Hello World", 45, 800);
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
