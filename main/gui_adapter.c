/*
 * gui_adapter.c
 *
 * Implementation of GUI Adapter and LVGL Task
 */

#include "gui_adapter.h"
#include "display_driver.h"
#include "app_state.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_log.h>
#include <string.h>

#include "lvgl.h"

static const char *TAG = "GuiAdapter";

#define UI_QUEUE_SIZE 10

static QueueHandle_t ui_event_queue = NULL;
static lv_obj_t *status_label = NULL;
static lv_obj_t *main_scr = NULL;

void ui_show_booting(void) {
    if (lv_disp_get_default() == NULL) return;
    
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Booting...");
    lv_obj_center(label);
}

void ui_show_home(void) {
    if (lv_disp_get_default() == NULL) return;

    if (main_scr == NULL) {
        main_scr = lv_obj_create(NULL);
        
        lv_obj_t *title = lv_label_create(main_scr);
        lv_label_set_text(title, "Matter Controller");
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

        status_label = lv_label_create(main_scr);
        lv_label_set_text(status_label, "Status: Idle");
        lv_obj_align(status_label, LV_ALIGN_CENTER, 0, 0);

        // Add a button for interaction test
        lv_obj_t *btn = lv_button_create(main_scr);
        lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -40);
        lv_obj_t *btn_label = lv_label_create(btn);
        lv_label_set_text(btn_label, "Test");
        lv_obj_center(btn_label);
    }
    
    lv_scr_load(main_scr);
}

void ui_update_status(const char* status) {
    if (status_label != NULL) {
        lv_label_set_text_fmt(status_label, "Status: %s", status);
    }
}

static void gui_task(void *pvParameter)
{
    ESP_LOGI(TAG, "GUI Task Started");
    
    // 2. Init Display (This initializes I2C, LVGL, Display, Touch)
    display_init();

    // 3. Show "Booting" Screen
    display_lock();
    ui_show_booting();
    display_unlock();

    UiEvent event;
    while (1) {
        // Run LVGL Timer is handled by esp_lvgl_port task. We just handle app events.

        // Check Mailbox
        if (xQueueReceive(ui_event_queue, &event, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "Received UI Event: %d", event.type);
            
            display_lock();
            switch (event.type) {
                case UI_EVENT_WIFI_CONNECTED:
                    ui_update_status("WiFi Connected");
                    break;
                case UI_EVENT_MATTER_COMMISSIONED:
                    ui_update_status("Matter Commissioned");
                    ui_show_home(); // Transition to Home
                    break;
                case UI_EVENT_SHOW_QR_CODE:
                    ESP_LOGI(TAG, "UI: Displaying QR Code: %s", event.data.qr_code.payload);
                    /* In a real app, generate QR code widget here */
                    if (status_label) lv_label_set_text(status_label, "Scan QR Code");
                    break;
                default:
                    break;
            }
            display_unlock();
        }
    }
}

esp_err_t gui_init(void)
{
    // 1. Create Queue
    ui_event_queue = xQueueCreate(UI_QUEUE_SIZE, sizeof(UiEvent));
    if (ui_event_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create UI queue");
        return ESP_FAIL;
    }

    // 2. Start Task
    // Pinning to Core 1 is often good for UI to avoid blocking Core 0 (WiFi/Networking)
    BaseType_t ret = xTaskCreatePinnedToCore(gui_task, "gui_task", 4096 * 2, NULL, 5, NULL, 1);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create GUI task");
        return ESP_FAIL;
    }

    return ESP_OK;
}

bool ui_send_event(UiEvent *event)
{
    if (ui_event_queue == NULL) {
        return false;
    }
    return (xQueueSend(ui_event_queue, event, 0) == pdTRUE);
}
