#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"

#include "mqtt_client.h"
#include "mqtt_app.h"

static const char *TAG = "mqtt_app";
static esp_mqtt_client_handle_t client = NULL;
static EventGroupHandle_t s_mqtt_event_group;
static SemaphoreHandle_t s_publish_mutex = NULL;
static int s_pending_publish_count = 0;
static char s_debug_topic[64] = {0};

#define MQTT_CONNECTED_BIT BIT0
#define MQTT_FAIL_BIT BIT1

void mqtt_app_publish_debug(const char *message)
{
    if (client != NULL && s_debug_topic[0] != '\0')
    {
        if (s_publish_mutex != NULL) {
            xSemaphoreTake(s_publish_mutex, portMAX_DELAY);
            s_pending_publish_count++;
            xSemaphoreGive(s_publish_mutex);
        }
        int msg_id = esp_mqtt_client_publish(client, s_debug_topic, message, 0, 1, 0);
        if (msg_id == -1) {
             if (s_publish_mutex != NULL) {
                xSemaphoreTake(s_publish_mutex, portMAX_DELAY);
                if (s_pending_publish_count > 0) s_pending_publish_count--;
                xSemaphoreGive(s_publish_mutex);
            }
        }
    }
}

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        xEventGroupSetBits(s_mqtt_event_group, MQTT_CONNECTED_BIT);
        mqtt_app_publish_debug("MQTT Connected");
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        if (s_publish_mutex != NULL) {
            xSemaphoreTake(s_publish_mutex, portMAX_DELAY);
            if (s_pending_publish_count > 0) {
                s_pending_publish_count--;
            }
            xSemaphoreGive(s_publish_mutex);
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        xEventGroupSetBits(s_mqtt_event_group, MQTT_FAIL_BIT);
        mqtt_app_publish_debug("MQTT Error");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

bool mqtt_app_start(const mqtt_config_t *config)
{
    if (config->debug_topic[0] != '\0')
    {
        strlcpy(s_debug_topic, config->debug_topic, sizeof(s_debug_topic));
    }

    s_mqtt_event_group = xEventGroupCreate();
    s_publish_mutex = xSemaphoreCreateMutex();
    char uri[128];
    snprintf(uri, sizeof(uri), "mqtt://%s:%d", config->broker_ip, config->port);

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .credentials.client_id = config->client_id,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    EventBits_t bits = xEventGroupWaitBits(s_mqtt_event_group,
                                           MQTT_CONNECTED_BIT | MQTT_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           pdMS_TO_TICKS(10000)); // Wait 10 seconds max

    if (bits & MQTT_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "MQTT Connected");
        return true;
    }
    else
    {
        ESP_LOGE(TAG, "MQTT Connection Failed or Timeout");
        return false;
    }
}

void mqtt_app_publish(const char *topic, const char *data)
{
    if (client != NULL)
    {
        if (s_publish_mutex != NULL) {
            xSemaphoreTake(s_publish_mutex, portMAX_DELAY);
            s_pending_publish_count++;
            xSemaphoreGive(s_publish_mutex);
        }

        int msg_id = esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
        
        if (msg_id == -1) {
             ESP_LOGE(TAG, "Failed to publish message");
             if (s_publish_mutex != NULL) {
                xSemaphoreTake(s_publish_mutex, portMAX_DELAY);
                if (s_pending_publish_count > 0) s_pending_publish_count--;
                xSemaphoreGive(s_publish_mutex);
            }
        } else {
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        }
    }
    else
    {
        ESP_LOGE(TAG, "MQTT client not initialized");
    }
}

bool mqtt_app_wait_all_published(int timeout_ms)
{
    int elapsed = 0;
    const int interval = 100;
    while (elapsed < timeout_ms) {
        int count = 0;
        if (s_publish_mutex != NULL) {
            xSemaphoreTake(s_publish_mutex, portMAX_DELAY);
            count = s_pending_publish_count;
            xSemaphoreGive(s_publish_mutex);
        }

        if (count == 0) {
            return true;
        }

        vTaskDelay(pdMS_TO_TICKS(interval));
        elapsed += interval;
    }
    return false;
}
