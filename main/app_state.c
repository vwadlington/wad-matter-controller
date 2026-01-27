/*
 * app_state.c
 *
 * Implementation of System State Management
 */

#include "app_state.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <esp_log.h>
#include <string.h>

static const char *TAG = "AppState";

static SystemState g_system_state;
static SemaphoreHandle_t g_state_mutex = NULL;

esp_err_t app_state_init(void)
{
    g_state_mutex = xSemaphoreCreateMutex();
    if (g_state_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create state mutex");
        return ESP_FAIL;
    }

    // Initialize default state
    memset(&g_system_state, 0, sizeof(SystemState));
    g_system_state.is_booting = true;
    g_system_state.wifi_connected = false;
    g_system_state.matter_commissioned = false;
    
    ESP_LOGI(TAG, "System State Initialized");
    return ESP_OK;
}

SystemState* app_state_get(void)
{
    return &g_system_state;
}

bool app_state_lock(void)
{
    if (g_state_mutex == NULL) {
        return false;
    }
    return (xSemaphoreTake(g_state_mutex, portMAX_DELAY) == pdTRUE);
}

void app_state_unlock(void)
{
    if (g_state_mutex != NULL) {
        xSemaphoreGive(g_state_mutex);
    }
}
