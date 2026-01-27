#include "display_driver.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "bsp/touch.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "driver/gpio.h"

static const char *TAG = "DISPLAY_DRIVER";

static lv_display_t *disp_handle = NULL;
static lv_indev_t *touch_handle = NULL;
static esp_lcd_touch_handle_t tp_handle = NULL;

void display_init(void)
{
    esp_err_t err = ESP_OK;

    // 1. Initialize I2C
    err = bsp_i2c_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C initialization failed!");
        return;
    }

    // 2. Initialize LVGL Port
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    err = lvgl_port_init(&lvgl_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "LVGL port initialization failed!");
        return;
    }

    // 3. Initialize Display Panel
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;
    
    err = bsp_display_new(NULL, &panel_handle, &io_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Display panel initialization failed!");
        return;
    }

    // 4. Register Display to LVGL
    // Configure for Double Buffering in PSRAM with Dirty Rectangles
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = 800 * 480, // Full frame buffer size for double buffering
        .double_buffer = true,    // allocate two buffers
        .hres = 800,
        .vres = 480,
        .monochrome = false,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = false,
            .buff_spiram = true,   // Allocate in PSRAM
            .sw_rotate = false,
            .swap_bytes = false,
            .full_refresh = false, // Allow partial updates (dirty rectangles)
            .direct_mode = false,
        }
    };
    
    const lvgl_port_display_rgb_cfg_t rgb_cfg = {
        .flags = {
             .bb_mode = true,
             .avoid_tearing = true,
        }
    };

    disp_handle = lvgl_port_add_disp_rgb(&disp_cfg, &rgb_cfg);
    if (disp_handle == NULL) {
        ESP_LOGE(TAG, "Failed to add RGB display to LVGL!");
        return;
    }

    // 5. Initialize Touch
    err = bsp_touch_new(NULL, &tp_handle);
    if (err == ESP_OK) {
        lvgl_port_touch_cfg_t touch_cfg = {0};
        touch_cfg.disp = disp_handle;
        touch_cfg.handle = tp_handle;
        touch_handle = lvgl_port_add_touch(&touch_cfg);
    } else {
        ESP_LOGE(TAG, "Touch initialization failed!");
    }

    // 6. Turn on Backlight
    // Try standard BSP API first
    bsp_display_backlight_on();
}

void display_lock(void)
{
    lvgl_port_lock(0);
}

void display_unlock(void)
{
    lvgl_port_unlock();
}

void display_rotate(lv_display_rotation_t rotation)
{
    if (disp_handle) {
        lv_disp_set_rotation(disp_handle, rotation);
    }
}
