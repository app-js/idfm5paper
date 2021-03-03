#include "M5EPD.h"

M5EPD_Canvas canvas(&M5.EPD);

void setup()
{
    log_d("idfm5: begin");
    M5.begin();
    log_d("idfm5: Clear");
    M5.EPD.Clear(true);
    log_d("idfm5: SetRotation");
    M5.EPD.SetRotation(90);
    log_d("idfm5: createCanvas");
    canvas.createCanvas(540,960);
    log_d("idfm5: setTextSize");
    canvas.setTextSize(3);
    log_d("idfm5: drawString");
    canvas.drawString("Hello World", 45, 100);
    canvas.drawString("Hello World", 45, 480);
    canvas.drawString("Hello World", 45, 860);
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
