/*
 * app_state.h
 *
 * System State Management
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

// Max number of Matter nodes we track comfortably
#define MAX_MATTER_NODES 10

typedef struct {
    uint64_t node_id;
    bool is_online;
    char name[32];
} MatterNode;

typedef struct {
    bool wifi_connected;
    bool matter_commissioned;
    char ip_address[16];
    
    // Matter Fabric / Nodes
    int node_count;
    MatterNode nodes[MAX_MATTER_NODES];
    
    // System Status
    bool is_booting;
} SystemState;

/**
 * @brief Initialize the global system state and mutex.
 * @return ESP_OK on success.
 */
esp_err_t app_state_init(void);

/**
 * @brief Get a pointer to the global system state.
 * @note  You MUST call app_state_lock() before accessing this pointer
 *        and app_state_unlock() after you are done.
 * @return Pointer to SystemState.
 */
SystemState* app_state_get(void);

/**
 * @brief Lock the system state mutex.
 * @return true if locked successfully, false otherwise.
 */
bool app_state_lock(void);

/**
 * @brief Unlock the system state mutex.
 */
void app_state_unlock(void);

#ifdef __cplusplus
}
#endif
