#pragma once

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

// Event Types for UI
enum class UiEventType {
    kMatterInitialized,
    kDeviceConnected,
    kDeviceDisconnected,
    kProvisioningQRCode,
    kInterfaceIpAddressChanged,
};

struct AppEvent {
    UiEventType type;
    union {
        struct {
            char payload[256];
        } qrCode;
        struct {
            uint64_t nodeId;
        } device;
    } data;
};

extern QueueHandle_t UiQueue;
