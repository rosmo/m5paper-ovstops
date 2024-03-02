#include "network.hpp"

int WifiNetwork::s_retry_num = 0;
EventGroupHandle_t WifiNetwork::s_wifi_event_group;

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (WifiNetwork::s_retry_num < 10) {
            esp_wifi_connect();
            WifiNetwork::s_retry_num++;
            printf("WiFI: retry to connect to the AP\n");
        } else {
            xEventGroupSetBits(WifiNetwork::s_wifi_event_group, WIFI_FAIL_BIT);
        }
        printf("WiFi: connect to the AP fail\n");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        printf("WiFi: Got IP address: " IPSTR "\n", IP2STR(&event->ip_info.ip));
        WifiNetwork::s_retry_num = 0;
        xEventGroupSetBits(WifiNetwork::s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void WifiNetwork::SetSSID(char *newSsid) {
    ssid = newSsid;
}

void  WifiNetwork::SetPassword(char *newPassword) {
    password = newPassword;
}

bool WifiNetwork::Connect() {
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = false;
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .threshold = {
                .authmode = WIFI_AUTH_WPA2_PSK,
            },
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        },
    };
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    printf("WiFi: wifi_init_sta finished.\n");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        printf("WiFi: connected to network SSID:%s\n",
                 ssid);
        return true;
    } else if (bits & WIFI_FAIL_BIT) {
        printf("WiFi: Failed to connect to SSID:%s, password:%s\n",
                 ssid, password);
    } else {
        printf("WiFi: UNEXPECTED EVENT\n");
    }
    return false;
}


   
