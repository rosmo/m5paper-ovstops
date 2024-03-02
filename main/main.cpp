/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"

// App specific
#include "nv_config.hpp"
#include "application.hpp"
#include "network.hpp"

#define TAG "Main"

NonVolatileConfig nvConfig;
OvstopsApplication app;
SemaphoreHandle_t applicationFinished = NULL;

void run_application(void *pvParameters) { 
    app.init();
    app.setup();
    app.run();
    if (applicationFinished != NULL) {
        printf("Application finished, releasing mutex...\n");
        xSemaphoreGive(applicationFinished);
    }
}

void heap_caps_alloc_failed_hook(size_t requested_size, uint32_t caps, const char *function_name)
{
  printf("%s was called but failed to allocate %d bytes with 0x%X capabilities. \n",function_name, requested_size, caps);
}

extern "C" void app_main(void) {
    heap_caps_register_failed_alloc_callback(heap_caps_alloc_failed_hook);

    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    // Load configuration from non-volatile storage
    nvConfig.loadNonvolatileConfiguration();
    if (nvConfig.isWifiConfigured()) {
        ESP_LOGI(TAG, "Wifi is configured, SSID=%s", nvConfig.getWifiSSID());
    } else {
        ESP_LOGI(TAG, "WiFi is not configured in NVS.");
    }
    
    // Create mutex for application
    // Semaphore is created as taken
    applicationFinished = xSemaphoreCreateBinary();

    WifiNetwork wifi;
    wifi.SetSSID(CONFIG_OVS_WIFI_SSID);
    wifi.SetPassword(CONFIG_OVS_WIFI_PASSWORD);
    ESP_LOGI(TAG, "Connecting to WiFi, SSID: %s", CONFIG_OVS_WIFI_SSID);
    wifi.Connect();

    // Create application task on core 0
    xTaskCreatePinnedToCore(run_application, "application", 4096, NULL, 1, NULL, 0);

    // Wait until task signals exit via semaphore
    xSemaphoreTake(applicationFinished, portMAX_DELAY); 

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}