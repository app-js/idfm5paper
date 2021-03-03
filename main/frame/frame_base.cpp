#include "frame_base.h"
#include "../epdgui/epdgui.h"

Frame_Base::Frame_Base(bool _has_title)
{
    if(_has_title)
    {
        _canvas_title = new M5EPD_Canvas(&M5.EPD);
        _canvas_title->createCanvas(540, 64);
        _canvas_title->drawFastHLine(0, 64, 540, 15);
        _canvas_title->drawFastHLine(0, 63, 540, 15);
        _canvas_title->drawFastHLine(0, 62, 540, 15);
        _canvas_title->setTextSize(26);
        _canvas_title->setTextDatum(CC_DATUM);
    }
    _frame_id = 0;
    _frame_name = "Frame_Base";
}

Frame_Base::~Frame_Base() 
{
    if(_key_exit != NULL)
        delete _key_exit;
    if(_canvas_title != NULL)
        delete _canvas_title;
}

void Frame_Base::exitbtn(String title, uint16_t width)
{
    _key_exit = new EPDGUI_Button(8, 12, width, 48);
    _key_exit->CanvasNormal()->fillCanvas(0);
    _key_exit->CanvasNormal()->setTextSize(26);
    _key_exit->CanvasNormal()->setTextDatum(CL_DATUM);
    _key_exit->CanvasNormal()->setTextColor(15);
    _key_exit->CanvasNormal()->drawString(title, 47 + 13, 28);
    _key_exit->CanvasNormal()->pushImage(15, 8, 32, 32, ImageResource_item_icon_arrow_l_32x32);
    *(_key_exit->CanvasPressed()) = *(_key_exit->CanvasNormal());
    _key_exit->CanvasPressed()->ReverseColor();
}

void Frame_Base::trbtn(String title, uint16_t width)
{
    _key_tr = new EPDGUI_Button(540-8-width, 12, width, 48);
    _key_tr->CanvasNormal()->fillCanvas(0);
    _key_tr->CanvasNormal()->setTextSize(26);
    _key_tr->CanvasNormal()->setTextDatum(CL_DATUM);
    _key_tr->CanvasNormal()->setTextColor(15);
    _key_tr->CanvasNormal()->drawString(title, 35, 28);
    *(_key_tr->CanvasPressed()) = *(_key_tr->CanvasNormal());
    _key_tr->CanvasPressed()->ReverseColor();
}

int Frame_Base::run(void)
{
    return _is_run;
}

void Frame_Base::exit(void)
{
}

void Frame_Base::exit_cb(epdgui_args_vector_t &args)
{
    EPDGUI_PopFrame();
    *((int*)(args[0])) = 0;
}

#include "esp_wifi.h"
#include "wifi_manager.h"

int Frame_Base::scan_reply{0};

void Frame_Base::cb_scan_ok(void *pvParameter)
{
	wifi_event_sta_scan_done_t* param = (wifi_event_sta_scan_done_t*)pvParameter;

	log_d("scan done with status %d", param->status);

    if (param->status == 0) scan_reply = 1;
    else scan_reply = 2;
}

int Frame_Base::connect_reply{0};

void Frame_Base::cb_connect_ok(void *pvParameter)
{
	ip_event_got_ip_t* param = (ip_event_got_ip_t*)pvParameter;

	/* transform IP to human readable string */
	char str_ip[16];
	esp_ip4addr_ntoa(&param->ip_info.ip, str_ip, IP4ADDR_STRLEN_MAX);

	log_d("I have a connection and my IP is %s!", str_ip);
    connect_reply = 1;
}

int Frame_Base::disconnect_reply{0};

void Frame_Base::cb_disconnect_ok(void *pvParameter)
{
	wifi_event_sta_disconnected_t* param = (wifi_event_sta_disconnected_t*)pvParameter;
	log_d("cb disconnect done from %s reason %d",param->ssid, param->reason);
    disconnect_reply = 1;
}
