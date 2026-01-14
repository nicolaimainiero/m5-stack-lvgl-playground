#include <M5Unified.h>

#define LV_CONF_INCLUDE_SIMPLE
#include <esp_timer.h>
#include <lvgl.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "core/m5-core.h"
#include "screens/screens.h"
#include "screens/level_screen.h"
#include "screens/screen_one.h"
#include "screens/weather_screen.h"

constexpr int32_t HOR_RES = 320;
constexpr int32_t VER_RES = 240;

constexpr auto SSID = "Solar Dynamics";
constexpr auto WIFI_PASSWORD = "Sa1lFastLiv3Slow";

lv_display_t *display;
lv_indev_t *touchpad;

static screen_state state;

void setup_wifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    M5.Log.print(".");
  }

  M5.Log.println("WiFi connected");
  M5.Log.print("IP address: ");
  M5.Log.printf("%s\n", WiFi.localIP().toString());


}


// continue setup code
void setup()
{
  M5.begin();
  lv_init();
  lv_tick_set_cb(tick_function);

  display = lv_display_create(HOR_RES, VER_RES);
  lv_display_set_flush_cb(display, display_flush);

  static lv_color_t buf1[HOR_RES * 15];
  lv_display_set_buffers(display, buf1, nullptr, sizeof(buf1),
                         LV_DISPLAY_RENDER_MODE_PARTIAL);

  touchpad = lv_indev_create();
  lv_indev_set_type(touchpad, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(touchpad, touchpad_read);

  setup_wifi();

  state.current_screen = 0;
  state.screens[0] = first_screen(state);
  state.screens[1] = weather_screen(state);
  state.screens[2] = level_screen(state);

  lv_screen_load(state.screens[state.current_screen]);
  update_temperature();
}

void loop()
{
  lv_task_handler();
  vTaskDelay(1);
}