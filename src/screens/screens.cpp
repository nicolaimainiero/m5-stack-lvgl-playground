#include "screens.h"

#include <M5Unified.h>
#include <math.h>
#include <lvgl.h>

void process_gesture(lv_event_t * e)
{
    screen_state* state = static_cast<screen_state*>(lv_event_get_user_data(e));
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

    switch(dir) {
        case LV_DIR_LEFT:
            state->current_screen = (state->current_screen + 1) % 3;
            lv_screen_load_anim(state->screens[state->current_screen], LV_SCREEN_LOAD_ANIM_MOVE_LEFT, 200, 0, false);
            break;
        case LV_DIR_RIGHT:
            state->current_screen = abs((state->current_screen - 1) % 3);
            lv_screen_load_anim(state->screens[state->current_screen], LV_SCREEN_LOAD_ANIM_MOVE_RIGHT, 200, 0, false);
            break;
        case LV_DIR_TOP:
            break;
        case LV_DIR_BOTTOM:
            break;
    }
}