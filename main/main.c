#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <stdio.h>


#define IDIOT_ESP_WIFI_SSID      "Всего идиотов: %d\n"
#define IDIOT_ESP_WIFI_CHANNEL   1
#define IDIOT_MAX_STA_CONN       10

static const char *TAG = "Idiot";
int boot_count = 0;
void wifi_init_softap(void);


void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
        esp_wifi_stop();  
        boot_count++;
        wifi_init_softap();
}

void wifi_init_softap(void)
{
    if(boot_count == 0)
    {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        WIFI_EVENT_AP_STACONNECTED,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));
    }
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = IDIOT_ESP_WIFI_SSID,
            .ssid_len = strlen(IDIOT_ESP_WIFI_SSID),
            .channel = IDIOT_ESP_WIFI_CHANNEL,
            .password = "",
            .max_connection = IDIOT_MAX_STA_CONN,
            .authmode = WIFI_AUTH_OPEN
        },
    };
    
    char  idiot_buffer[40];    
    snprintf(idiot_buffer, sizeof(idiot_buffer), IDIOT_ESP_WIFI_SSID, boot_count);
    size_t ssid_length = strlen(idiot_buffer);
    memcpy(wifi_config.ap.ssid, idiot_buffer, ssid_length);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "%s %d", idiot_buffer, boot_count);
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();
    
    wifi_init_softap();
}