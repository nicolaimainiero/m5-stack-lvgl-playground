#include "weather_screen.h"
#include <lvgl.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

static lv_subject_t temperature;
static lv_subject_t times[5];
static lv_subject_t forecasts[5];

static char buf[8];
static char prev_buf[8];
static size_t size = 8;

void update_temperature()
{
    WiFiClient client;
    HTTPClient http;
    M5.Log.println("Getting current weather conditions.");
    // Send request
    http.begin(client, "http://api.open-meteo.com/v1/forecast?latitude=48.728888888889&longitude=9.2586111111111&current=temperature_2m&hourly=temperature_2m");
    http.GET();

    // Print the response
    JsonDocument doc;
    deserializeJson(doc, http.getString());
    M5.Log.printf("Temperature %.1f °C", doc["current"]["temperature_2m"].as<float>());
    M5.Log.printf(" at %s\n", doc["current"]["time"].as<const char *>());

    lv_subject_set_int(&temperature, lroundf(doc["current"]["temperature_2m"].as<float>()));

    const char *target = doc["current"]["time"].as<const char *>();
    JsonArray arr = doc["hourly"]["time"];
    JsonArray forcast = doc["hourly"]["temperature_2m"];
    std::vector<int> indices;
    indices.reserve(5);

    M5.Log.printf("%d Eintraege\n", arr.size());
    // Array durchlaufen
    for (size_t i = 0; i < arr.size(); i++)
    {
        const char *t = arr[i];

        if (strcmp(t, target) > 0)
        { // lexikografischer ISO-Vergleich
            indices.push_back(i);
            if (indices.size() == 5)
                break;
        }
    }

    uint8_t i = 0;
    for (int idx : indices)
    {
        char time_value[8];
        strncpy(time_value, arr[idx].as<const char *>() + 11, 5);

        M5.Log.printf("%d\n", idx);
        M5.Log.printf("%s - %s: ", arr[idx].as<const char *>(), time_value);
        M5.Log.printf("%.1f\n", forcast[idx].as<float>());

        lv_subject_copy_string(&times[i], time_value);
        lv_subject_set_int(&forecasts[i], lround(forcast[idx].as<float>()));
        i++;
    }

    // Disconnect
    http.end();
}

void update_temperature_cb(lv_timer_t *timer)
{
    update_temperature();
}

lv_obj_t *weather_screen(screen_state &state)
{
    lv_subject_init_int(&temperature, 10);
    for (uint8_t i = 0; i < 5; i++)
    {
        lv_subject_init_int(&forecasts[i], 10);
        lv_subject_init_string(&times[i], buf, prev_buf, size, "10:00");
    }

    lv_obj_t *screen = lv_obj_create(NULL);

    // background image
    LV_IMAGE_DECLARE(sunny_small);
    lv_obj_t *background = lv_image_create(screen);
    lv_image_set_src(background, &sunny_small);
    lv_obj_align(background, LV_ALIGN_CENTER, 0, 0);

    // style the unit label
    static lv_style_t unit_label_style;
    lv_style_init(&unit_label_style);
    lv_style_set_pad_right(&unit_label_style, 0);
    lv_style_set_text_color(&unit_label_style, lv_color_hex(0xF1F5F7));
    lv_style_set_text_font(&unit_label_style, &lv_font_montserrat_28);

    // the unit label
    lv_obj_t *unit_label = lv_label_create(screen);
    lv_obj_align(unit_label, LV_ALIGN_RIGHT_MID, -4, -22);
    lv_label_set_text(unit_label, "°C");
    lv_obj_add_style(unit_label, &unit_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);

    // style the temperature label
    static lv_style_t temp_label_style;
    lv_style_init(&temp_label_style);
    lv_style_set_text_color(&temp_label_style, lv_color_hex(0xF1F5F7));
    lv_style_set_text_font(&temp_label_style, &lv_font_montserrat_48);
    lv_style_set_text_align(&temp_label_style, LV_TEXT_ALIGN_RIGHT);

    // the temperature label
    lv_obj_t *temp_label = lv_label_create(screen);
    lv_label_bind_text(temp_label, &temperature, "%d");
    lv_obj_align_to(temp_label, unit_label, LV_ALIGN_OUT_LEFT_TOP, -20, 0);
    lv_obj_set_size(temp_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_style(temp_label, &temp_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);

    // style the forcast grid
    static lv_style_t forecast_row_style;
    lv_style_init(&forecast_row_style);
    lv_style_set_border_width(&forecast_row_style, 1);
    lv_style_set_border_side(&forecast_row_style, LV_BORDER_SIDE_INTERNAL);
    lv_style_set_border_color(&forecast_row_style, lv_color_hex(0x444444));
    lv_style_set_bg_color(&forecast_row_style, lv_color_hex(0x333333));
    lv_style_set_bg_opa(&forecast_row_style, LV_OPA_40);

    // forecast container with flex layout
    lv_obj_t *forcast_row = lv_obj_create(screen);
    lv_obj_set_size(forcast_row, 300, 80);
    lv_obj_align(forcast_row, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_flex_flow(forcast_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(forcast_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_add_style(forcast_row, &forecast_row_style, LV_PART_MAIN | LV_STATE_DEFAULT);

    // style for the forecast cell
    static lv_style_t forecast_cell_style;
    lv_style_init(&forecast_cell_style);
    lv_style_set_border_width(&forecast_cell_style, 0);
    lv_style_set_bg_opa(&forecast_cell_style, LV_OPA_TRANSP);
    lv_style_set_pad_all(&forecast_cell_style, 0);

    // style the forcast labels
    static lv_style_t forecast_label_style;
    lv_style_init(&forecast_label_style);
    lv_style_set_bg_opa(&forecast_label_style, LV_OPA_TRANSP);
    lv_style_set_text_align(&forecast_label_style, LV_TEXT_ALIGN_RIGHT);
    lv_style_set_text_color(&forecast_label_style, lv_color_hex(0xF1F5F7));
  //  lv_style_set_border_width(&forecast_label_style, 1);

    // forecast labels
    lv_obj_t *fc_label_temp;
    lv_obj_t *fc_label_time;
    for (uint8_t i = 0; i < 5; i++)
    {
        lv_obj_t *forcast_cell = lv_obj_create(forcast_row);
        lv_obj_add_style(forcast_cell, &forecast_cell_style, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_size(forcast_cell, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

        fc_label_time = lv_label_create(forcast_cell);
        lv_obj_set_size(fc_label_time, 50, LV_SIZE_CONTENT);
        lv_label_bind_text(fc_label_time, &times[i], "%s");
        lv_obj_add_style(fc_label_time, &forecast_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(fc_label_time, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

        fc_label_temp = lv_label_create(forcast_cell);
        lv_obj_set_size(fc_label_temp, 50, LV_SIZE_CONTENT);
        lv_obj_align_to(fc_label_temp, fc_label_time, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
        lv_label_bind_text(fc_label_temp, &forecasts[i], "%d°");
        lv_obj_add_style(fc_label_temp, &forecast_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(fc_label_temp, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    // gesture handling for changing screens
    lv_obj_add_event_cb(screen, process_gesture, LV_EVENT_GESTURE, &state);

    // timer to update weather information
    lv_timer_create(update_temperature_cb, 60 * 60 * 1000, nullptr);

    return screen;
}