/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_matter.h>
#include <esp_matter_controller_client.h>
#include <esp_matter_controller_utils.h>
#include <esp_matter_ota.h>
#include <app/server/Server.h>
#include <credentials/FabricTable.h>
#include <setup_payload/SetupPayload.h>
#include <setup_payload/QRCodeSetupPayloadGenerator.h>

#if CONFIG_OPENTHREAD_BORDER_ROUTER
#include <esp_openthread_border_router.h>
#include <esp_openthread_lock.h>
#include <esp_ot_config.h>
#include <esp_spiffs.h>
#include <platform/ESP32/OpenthreadLauncher.h>
#endif // CONFIG_OPENTHREAD_BORDER_ROUTER

#include <app_reset.h>
#include <common_macros.h>

#include "app_state.h"
#include "gui_adapter.h"

static const char *TAG = "app_main";

static void app_event_cb(const chip::DeviceLayer::ChipDeviceEvent *event, intptr_t arg)
{
    UiEvent uiEvent;
    uiEvent.type = UI_EVENT_NONE;
    bool sendEvent = false;

    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::PublicEventTypes::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP Address changed");
        // Update System State
        if (app_state_lock()) {
            SystemState *state = app_state_get();
            state->wifi_connected = true;
            // TODO: Copy IP address
            app_state_unlock();
        }

        uiEvent.type = UI_EVENT_WIFI_CONNECTED;
        sendEvent = true;
        break;

    case chip::DeviceLayer::DeviceEventType::kESPSystemEvent:
        if (event->Platform.ESPSystemEvent.Base == IP_EVENT &&
            event->Platform.ESPSystemEvent.Id == IP_EVENT_STA_GOT_IP) {
#if CONFIG_OPENTHREAD_BORDER_ROUTER
            static bool sThreadBRInitialized = false;
            if (!sThreadBRInitialized) {
                esp_openthread_set_backbone_netif(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"));
                esp_openthread_lock_acquire(portMAX_DELAY);
                esp_openthread_border_router_init();
                esp_openthread_lock_release();
                sThreadBRInitialized = true;
            }
#endif
        }
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning Complete");
        if (app_state_lock()) {
            SystemState *state = app_state_get();
            state->matter_commissioned = true;
            app_state_unlock();
        }
        uiEvent.type = UI_EVENT_MATTER_COMMISSIONED;
        sendEvent = true;
        break;
        
    default:
        break;
    }

    if (sendEvent) {
        if (!ui_send_event(&uiEvent)) {
            ESP_LOGW(TAG, "Failed to send event to UI Queue");
        }
    }
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    ESP_LOGI(TAG, "Starting System...");

    /* 1. Init NVS */
    nvs_flash_init();

    /* 2. Init App State */
    err = app_state_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init app state");
        return;
    }

    /* 3. Init Display & UI */
    err = gui_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init GUI");
        return;
    }

    /* 4. Init WiFi/Matter */
    err = esp_matter::start(app_event_cb);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start Matter, err:%d", err);
        return;
    }

    /* Provisioning Setup / QR Code Display */
#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    // Generate QR Code Payload
    chip::PayloadContents payloadContents;
    CHIP_ERROR chipErr = chip::DeviceLayer::GetPayloadContents(payloadContents, chip::DeviceLayer::CommissioningWindowAdvertisementBit::kInternal);
    if (chipErr == CHIP_NO_ERROR) {
        char qrCodeBuffer[256];
        size_t qrCodeLen = sizeof(qrCodeBuffer);
        
        chipErr = chip::QRCodeSetupPayloadGenerator(payloadContents).payloadBase38Representation(
            qrCodeBuffer, qrCodeLen);
            
        if (chipErr == CHIP_NO_ERROR) {
            ESP_LOGI(TAG, "QR Code Payload: %s", qrCodeBuffer);
            
            UiEvent qrEvent;
            qrEvent.type = UI_EVENT_SHOW_QR_CODE;
            strncpy(qrEvent.data.qr_code.payload, qrCodeBuffer, sizeof(qrEvent.data.qr_code.payload) - 1);
            qrEvent.data.qr_code.payload[sizeof(qrEvent.data.qr_code.payload) - 1] = 0;
            
            if (!ui_send_event(&qrEvent)) {
                ESP_LOGW(TAG, "Failed to send QR Code to UI Queue");
            }
        } else {
            ESP_LOGE(TAG, "Failed to generate QR Code payload");
        }
    } else {
        ESP_LOGE(TAG, "Failed to get payload contents");
    }

    esp_matter::lock::chip_stack_lock(portMAX_DELAY);
    esp_matter::controller::matter_controller_client::get_instance().init(112233, 1, 5580);
    esp_matter::controller::matter_controller_client::get_instance().setup_commissioner();
    esp_matter::lock::chip_stack_unlock();
#endif
}
