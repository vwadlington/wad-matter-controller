/*
 * gui_adapter.h
 *
 * Interface for the GUI Adapter (Mailbox Pattern)
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UI_EVENT_NONE,
    UI_EVENT_MATTER_INIT,
    UI_EVENT_WIFI_CONNECTED,
    UI_EVENT_MATTER_COMMISSIONED,
    UI_EVENT_DEVICE_CONNECTED,
    UI_EVENT_DEVICE_DISCONNECTED,
    UI_EVENT_SHOW_QR_CODE
} UiEventType;

typedef struct {
    UiEventType type;
    union {
        struct {
            char payload[256];
        } qr_code;
        struct {
            uint64_t node_id;
        } device;
    } data;
} UiEvent;

/**
 * @brief Initialize the GUI system, LVGL, and the UI task.
 * @return ESP_OK on success.
 */
esp_err_t gui_init(void);

/**
 * @brief Send an event to the UI task (Mailbox).
 * @param event Pointer to the UiEvent to send.
 * @return true if sent successfully, false if queue full.
 */
bool ui_send_event(UiEvent *event);

#ifdef __cplusplus
}
#endif
