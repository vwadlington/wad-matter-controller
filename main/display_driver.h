#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void display_init(void);
void display_lock(void);
void display_unlock(void);
void display_rotate(lv_display_rotation_t rotation);

#ifdef __cplusplus
}
#endif
