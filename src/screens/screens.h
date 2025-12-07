#include <lvgl.h>

#ifndef SCREEN_STATE_H // header guard
#define SCREEN_STATE_H

struct screen_state
{
    int current_screen;
    lv_obj_t *screens[2];
};

#endif // PARSER_H

void process_gesture(lv_event_t *e);