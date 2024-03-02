#ifndef __NETWORK_H
#define __NETWORK_H
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

class WifiNetwork {
    public:
        WifiNetwork() { };
        void SetSSID(char *newSsid);
        void SetPassword(char *newPassword);
        bool Connect();
        static EventGroupHandle_t s_wifi_event_group;
        static int s_retry_num;
    private:
        char *ssid;
        char *password;
};

#endif