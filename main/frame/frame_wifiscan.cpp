#include "frame_wifiscan.h"
#include "frame_wifipassword.h"
#include "esp_wifi.h"
#include "wifi_manager.h"

#define MAX_BTN_NUM     14
#define MAX_WIFI_NUM    (MAX_BTN_NUM - 1)
bool _update_flag = false;
EPDGUI_Button *_connect_key = NULL;

const uint8_t *kIMGWifiLevel[4] = {
    NULL,
    ImageResource_item_icon_wifi_1_32x32,
    ImageResource_item_icon_wifi_2_32x32,
    ImageResource_item_icon_wifi_3_32x32
};

EPDGUI_Button *Frame_WifiScan::_key_wifi[14];

void key_wifi_cb(epdgui_args_vector_t &args)
{
    if(((EPDGUI_Button*)(args[0]))->GetCustomString() == "_$refresh$_")
    {
        _update_flag = true;
    }
    else
    {
        _connect_key = (EPDGUI_Button*)(args[0]);
        Frame_Base *frame = EPDGUI_GetFrame("Frame_WifiPassword");
        if(frame == NULL)
        {
            frame = new Frame_WifiPassword(false);
            EPDGUI_AddFrame("Frame_WifiPassword", frame);
        }
        EPDGUI_PushFrame(frame);
        *((int*)(args[1])) = 0;
    }
}

Frame_WifiScan::Frame_WifiScan(void)
{
    _frame_name = "Frame_WifiScan";

    for(int i = 0; i < MAX_BTN_NUM; i++)
    {
        _key_wifi[i] = new EPDGUI_Button(4, 100 + i * 60, 532, 61);
        _key_wifi[i]->SetHide(true);
        _key_wifi[i]->CanvasNormal()->setTextSize(26);
        _key_wifi[i]->CanvasNormal()->setTextDatum(CL_DATUM);
        _key_wifi[i]->CanvasNormal()->setTextColor(15);
        _key_wifi[i]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _key_wifi[i]);
        _key_wifi[i]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, (void*)(&_is_run));
        _key_wifi[i]->Bind(EPDGUI_Button::EVENT_RELEASED, key_wifi_cb);
    }
 
    _canvas_title->drawString("WLAN", 270, 34);

    exitbtn("Home");
    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &Frame_Base::exit_cb);

    trbtn("Disconnect");
    _key_tr->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_connected));
    _key_tr->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, (void*)(&_update_flag));
    _key_tr->AddArgs(EPDGUI_Button::EVENT_RELEASED, 2, (void*)(&_connect_ssid));
    _key_tr->Bind(EPDGUI_Button::EVENT_RELEASED, &disconnect);

    _update_flag = true;
    _connected = 0;
}

Frame_WifiScan::~Frame_WifiScan(void)
{
    log_d("Frame_WifiScan::~Frame_WifiScan(void)");
    for(int i = 0; i < MAX_BTN_NUM; i++)
    {
        delete _key_wifi[i];
    }
}

void Frame_WifiScan::moveButtons(int ypos)
{
    log_d("Frame_WifiScan::moveButtons(%d)",ypos);
    for(int i = 1; i < MAX_BTN_NUM; i++)
    {
        _key_wifi[i]->SetPos(_key_wifi[i]->getX(), _key_wifi[i]->getY() + ypos);
    }
}

void Frame_WifiScan::resetButtons()
{
    log_d("Frame_WifiScan::resetButtons()");
    for(int i = 0; i < MAX_BTN_NUM; i++)
    {
        _key_wifi[i]->SetPos(4, 100 + i * 60);
    }
}

void Frame_WifiScan::disconnect(epdgui_args_vector_t &args)
{
    log_d("tr button disconnect");
    if ( *((uint8_t*)(args[0])) )
    {
        log_d("start disconnect");
        wifi_manager_set_callback(WM_EVENT_STA_DISCONNECTED, &cb_disconnect_ok);
        disconnect_reply = 0;
        wifi_manager_disconnect_async();

        uint32_t start_time  = millis();
        while (disconnect_reply == 0)
        {
            if (millis() - start_time > 15000)
            {
                log_d("disconnect timeout");
                break;
            }
            sleep(1);
        }

        log_d("disconnect disconnected");

        for(int i = 0; i < MAX_BTN_NUM; i++)
        {
            _key_wifi[i]->SetPos(4, 100 + i * 60);
            _key_wifi[i]->SetEnable(true);
        }
        *((uint8_t*)(args[0])) = 0;
        *((String*)(args[2])) = "";
        *((bool*)(args[1])) = true;
    }
    else
       log_d("not connected");
    log_d("disconnect done");
}

void Frame_WifiScan::DrawItem(EPDGUI_Button *btn, String ssid, int rssi)
{
    log_d("Frame_WifiScan::DrawItem(%s, %d)",ssid.c_str(),rssi);

    int level = 0;
    if(rssi > -55)
    {
        level = 3;
    }
    else if(rssi > -88)
    {
        level = 2;
    }
    else
    {
        level = 1;
    }
    if(ssid.length() > 22)
    {
        ssid = ssid.substring(0, 22) + "...";
    }
    btn->SetHide(false);
    btn->CanvasNormal()->fillCanvas(0);
    btn->CanvasNormal()->drawRect(0, 0, 532, 61, 15);
    btn->CanvasNormal()->drawString(ssid, 15, 35);
    btn->SetCustomString(ssid);
    btn->CanvasNormal()->pushImage(532 - 15 - 32, 14, 32, 32, kIMGWifiLevel[level]);
    *(btn->CanvasPressed()) = *(btn->CanvasNormal());
    btn->CanvasPressed()->ReverseColor();
}

int Frame_WifiScan::run()
{
    // log_d("Frame_WifiScan::run()");
    if(_connect)
    {
        log_d("Connect()");
        _connect = false;
        Connect();
    }
    if(_update_flag)
    {
        log_d("scan()");
        _update_flag = false;
        scan();
    }
    return 1;
}

int Frame_WifiScan::scan()
{
    log_d("Frame_WifiScan::scan()");
    uint16_t wifi_num = 0;
    wifi_ap_record_t ap;

	wifi_manager_set_callback(WM_EVENT_SCAN_DONE, &cb_scan_ok);
    scan_reply = 0;
    
    wifi_manager_scan_async();

    uint32_t start_time = millis();
    while (scan_reply == 0)
    {
        if (millis() - start_time > 15000)
        {
            return 0;
        }
        sleep(1);
    }

    if (scan_reply >= 2)
    {
        log_d("Frame_WifiScan::scan() failed");
        return 0;
    }

    wifi_num = wifi_manager_get_anum();
    log_d("Frame_WifiScan::wifi_num = %d _connected = %d",wifi_num,_connected);

    if(_scan_count > 0)
    {
        M5.EPD.WriteFullGram4bpp(GetWallpaper());
        _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
        _key_exit->Draw(UPDATE_MODE_NONE);
        _key_tr->Draw(UPDATE_MODE_NONE);
        M5.EPD.UpdateFull(UPDATE_MODE_GC16);
    }
    _scan_count++;

    int connect_wifi_idx = -1;
    if(_connected)
    {
        for(int i = 0; i < wifi_num; i++)
        {
            ap = wifi_manager_get_ap(i);
            std::string str((char*)(ap.ssid));
            String ssid(str.c_str());
            
            if(_connect_ssid.length() >= 1 && ssid == _connect_ssid)
            {
                connect_wifi_idx = i;
                if(ap.rssi < -90)
                {
                    connect_wifi_idx = -1;
                }
                break;
            }
        }
        if(connect_wifi_idx == -1)
        {
            Discon();
            _key_wifi[0]->SetEnable(true);
            _connected = 0;
            moveButtons(-32);
        }
    }

    wifi_num = wifi_num > MAX_WIFI_NUM ? MAX_WIFI_NUM : wifi_num;

    for(int i = _connected; i < MAX_BTN_NUM; i++)
    {
        // log_d("Frame_WifiScan SetHide %d %x %s %s",i,*(_key_wifi+sizeof(EPDGUI_Button *)*i),_key_wifi[i]->getLabel().c_str(),_key_wifi[i]->GetCustomString().c_str());
        _key_wifi[i]->SetHide(true);
    }

    if(_connected)
    {
        _key_wifi[0]->Draw(UPDATE_MODE_A2);
    }

    int idx = 0;
    while(wifi_num >= 1)
    {
        log_d("wifi_num %d, _connected %d, connect_wifi_idx %d, idx %d",wifi_num, _connected, connect_wifi_idx, idx);
        if(idx == connect_wifi_idx)
        {
            idx++;
            continue;
        }

        ap = wifi_manager_get_ap(idx);

        std::string str((char*)(ap.ssid));
        String ssid(str.c_str());
        DrawItem(_key_wifi[idx], ssid, ap.rssi);
        _key_wifi[idx]->Draw(UPDATE_MODE_A2);

        idx++;
        if(idx == wifi_num || (_connected && idx == MAX_WIFI_NUM-1))
            break;
    }

    _key_wifi[idx]->SetCustomString("_$refresh$_");
    _key_wifi[idx]->SetHide(false);
    _key_wifi[idx]->CanvasNormal()->fillCanvas(0);
    _key_wifi[idx]->CanvasNormal()->drawRect(0, 0, 532, 61, 15);
    _key_wifi[idx]->CanvasNormal()->pushImage(15, 14, 32, 32, ImageResource_item_icon_refresh_32x32);
    _key_wifi[idx]->CanvasNormal()->drawString("Refresh", 58, 35);
    *(_key_wifi[idx]->CanvasPressed()) = *(_key_wifi[idx]->CanvasNormal());
    _key_wifi[idx]->CanvasPressed()->ReverseColor();
    _key_wifi[idx]->Draw(UPDATE_MODE_A2);

    M5.EPD.UpdateFull(UPDATE_MODE_GL16);
    // M5.EPD.UpdateArea(0, 64, 540, 72, UPDATE_MODE_GL16);

    return 0;
}

void Frame_WifiScan::Discon()
{
    log_d("Frame_WifiScan::Discon()");
    bool dummy;
    epdgui_args_vector_t dargs = {(void*)(&_connected),(void*)(&dummy),(void*)(&_connect_ssid)};
    disconnect(dargs);
}

void Frame_WifiScan::Connect()
{
    log_d("Frame_WifiScan::Connect()");
    uint32_t start_time;
    int anime_cnt = 0;
    int x = 532 - 15 - 32;
    int y = _connect_key->getY() + 14;
    M5EPD_Canvas loading(&M5.EPD);
    loading.createCanvas(32, 32);
    loading.fillCanvas(0);
    loading.pushCanvas(x, y, UPDATE_MODE_GL16);
    _connect_ssid = _connect_key->GetCustomString();
    log_d("SSID = [%s] PSWD = [%s]", _connect_ssid.c_str(), _connect_password.c_str());

    if (_connected)
    {
        Discon();
        _connect_ssid = _connect_key->GetCustomString();
    }
    
	wifi_manager_set_callback(WM_EVENT_STA_GOT_IP, &cb_connect_ok);
    connect_reply = 0;
    wifi_manager_set_wifi_sta_config(_connect_ssid.c_str(), _connect_password.c_str());
    wifi_manager_connect_async();

    start_time = millis();
    while (connect_reply == 0)
    {
        loading.pushImage(0, 0, 32, 32, GetLoadingIMG_32x32(anime_cnt));
        loading.pushCanvas(x, y, UPDATE_MODE_DU4);
        anime_cnt++;
        if(anime_cnt == 16)
        {
            anime_cnt = 0;
        }

        if (millis() - start_time > 10000)
        {
            M5EPD_Canvas err(&M5.EPD);
            err.createCanvas(300, 100);
            err.fillCanvas(15);
            err.setTextSize(26);
            err.setTextColor(0);
            err.setTextDatum(CC_DATUM);
            err.drawString("Wrong password", 150, 55);
            err.pushCanvas(120, 430, UPDATE_MODE_GL16);
            _connected = 0;
            scan();
            return;
        }
    }

    moveButtons(32);

    _connect_key->CanvasNormal()->pushImage(532 - 15 - 32, 14, 32, 32, ImageResource_item_icon_success_32x32);
    
    _key_wifi[0]->SetEnable(false);
    _key_wifi[0]->SetHide(false);
    if(_connect_key != _key_wifi[0])
    {
        *(_key_wifi[0]->CanvasNormal()) = *(_connect_key->CanvasNormal());
        *(_key_wifi[0]->CanvasPressed()) = *(_connect_key->CanvasNormal());
        _key_wifi[0]->CanvasPressed()->ReverseColor();
    }
    loading.pushImage(0, 0, 32, 32, ImageResource_item_icon_success_32x32);
    loading.pushCanvas(x, y, UPDATE_MODE_GL16);
    _connected = 1;

    SetWifi(_connect_ssid, _connect_password);
    SyncNTPTime();
    scan();
}

void Frame_WifiScan::SetConnected(String ssid, int rssi)
{
    _connect_ssid = ssid;
    DrawItem(_key_wifi[0], ssid, rssi);
    moveButtons(32);
    _key_wifi[0]->SetEnable(false);
    _key_wifi[0]->SetHide(false);
    _connected = 1;
}

int Frame_WifiScan::init(epdgui_args_vector_t &args)
{
    log_d("Frame_WifiScan::init %d", args.size());
    _is_run = 1;
    _connect = false;
    M5.EPD.WriteFullGram4bpp(GetWallpaper());
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    if(args.size() > 0)
    {
        String *password = (String*)(args[0]);
        _connect_password = *password;
        delete password;
        args.pop_back();

        for(int i = 0; i < MAX_BTN_NUM; i++)
        {
            EPDGUI_AddObject(_key_wifi[i]);
        }
        _update_flag = false;
        _connect = true;
    }
    else
    {
        for(int i = 0; i < MAX_BTN_NUM; i++)
        {
            _key_wifi[i]->SetHide(true);
            EPDGUI_AddObject(_key_wifi[i]);
        }
        if(_connected)
        {
            _key_wifi[0]->SetHide(false);
        }
        _update_flag = true;
        _scan_count = 0;
        _connect = false;
    }
    EPDGUI_AddObject(_key_exit);
    EPDGUI_AddObject(_key_tr);
    
    return 3;
}