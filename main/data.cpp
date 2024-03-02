#include "data.hpp"

#define MAX_HTTP_OUTPUT_BUFFER 16384
#define TAG "StopsData"

#define DATA_LOAD_FREQUENCY 45

void StopsData::SetUrl(const char *url) {
    dataUrl = url;
}

void StopsData::SetNotifyTask(TaskHandle_t task_) {
    task = task_;
}

esp_err_t StopsData::HttpEventHandler(esp_http_client_event_t *evt)
{
    static char *output_buffer; // Buffer to store response of http request from event handler
    static int output_len;      // Stores number of bytes read
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(TAG, "HTTP: error!");
        break;
    case HTTP_EVENT_ON_CONNECTED:
    case HTTP_EVENT_HEADER_SENT:
    case HTTP_EVENT_ON_HEADER:
        break;
    case HTTP_EVENT_ON_DATA:
        // Clean the buffer in case of a new request
        if (output_len == 0 && evt->user_data)
        {
            // we are just starting to copy the output data into the use
            memset(evt->user_data, 0, MAX_HTTP_OUTPUT_BUFFER);
        }
        /*
         *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
         *  However, event handler can also be used in case chunked encoding is used.
         */
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            // If user_data buffer is configured, copy the response into the buffer
            int copy_len = 0;
            if (evt->user_data)
            {
                // The last byte in evt->user_data is kept for the NULL character in case of out-of-bound access.
                copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
                if (copy_len)
                {
                    memcpy((char *)evt->user_data + output_len, evt->data, copy_len);
                }
            }
            else
            {
                int content_len = esp_http_client_get_content_length(evt->client);
                if (output_buffer == NULL)
                {
                    // We initialize output_buffer with 0 because it is used by strlen() and similar functions therefore should be null terminated.
                    output_buffer = (char *)calloc(content_len + 1, sizeof(char));
                    output_len = 0;
                    if (output_buffer == NULL)
                    {
                        ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                        return ESP_FAIL;
                    }
                }
                copy_len = MIN(evt->data_len, (content_len - output_len));
                if (copy_len)
                {
                    memcpy(output_buffer + output_len, evt->data, copy_len);
                }
            }
            output_len += copy_len;
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        if (output_buffer != NULL)
        {
            // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
            // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP: disconnected");
        if (output_buffer != NULL)
        {
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    case HTTP_EVENT_REDIRECT:
        esp_http_client_set_redirection(evt->client);
        break;
    }
    return ESP_OK;
}

StopData::StopData(cJSON *stop) {
    cJSON *lineNo_ = cJSON_GetObjectItemCaseSensitive(stop, "line_no");
    cJSON *lineName_ = cJSON_GetObjectItemCaseSensitive(stop, "line");
    cJSON *destination_ = cJSON_GetObjectItemCaseSensitive(stop, "destination");
    cJSON *transportType_ = cJSON_GetObjectItemCaseSensitive(stop, "transport");
    cJSON *stopName_ = cJSON_GetObjectItemCaseSensitive(stop, "stop");
    cJSON *status_ = cJSON_GetObjectItemCaseSensitive(stop, "status");
    cJSON *timeTarget_ = cJSON_GetObjectItemCaseSensitive(stop, "time_target");
    cJSON *timeEstimated_ = cJSON_GetObjectItemCaseSensitive(stop, "time_estimated");
    cJSON *timeDifference_ = cJSON_GetObjectItemCaseSensitive(stop, "time_diff");

    lineNo = lineNo_ != NULL ? strdup(lineNo_->valuestring) : strdup("");
    lineName = lineName_ != NULL ? strdup(lineName_->valuestring) : strdup("");
    destination = destination_ != NULL ? strdup(destination_->valuestring) : strdup("");
    transportType = transportType_ != NULL ? strdup(transportType_->valuestring) : strdup("");
    stopName = stopName_ != NULL ? strdup(stopName_->valuestring) : strdup("");
    status = status_ != NULL ? strdup(status_->valuestring) : strdup("");
    timeTarget = timeTarget_ != NULL ? (int64_t)timeTarget_->valuedouble : (int64_t)0;
    timeEstimated = timeEstimated_ != NULL ? (int64_t)timeEstimated_->valuedouble : (int64_t)0;
    timeDifference = timeDifference_ != NULL ? (int64_t)timeDifference_->valuedouble : (int64_t)0;
}

void StopData::Print() {
    ESP_LOGI(TAG, "%s %s (%s) to %s, status %s - estimated %s", transportType, lineNo, lineName, destination, status, this->GetTimeEstimated());
}

StopData::~StopData() {
    ESP_LOGI(TAG, "~StopData()");
    free(lineNo);
    free(lineName);
    free(destination);
    free(transportType);
    free(stopName);
    free(status);
}

esp_err_t StopsData::Load()
{
    char *local_response_buffer = NULL;
    char url[256] = {'\0'};
    local_response_buffer = (char *)malloc(MAX_HTTP_OUTPUT_BUFFER);
    if (local_response_buffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate response buffer: %d", MAX_HTTP_OUTPUT_BUFFER);
        return ESP_FAIL;
    }

    loaded = false;

    esp_err_t err;
    snprintf(url, 256, "%s", dataUrl);
    ESP_LOGI(TAG, "Fetching stop data from: %s", url);
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .timeout_ms = 3000,
        .event_handler = HttpEventHandler,
        .user_data = local_response_buffer,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Accept", "application/json");
    esp_http_client_set_header(client, "Cache-control", "no-cache");
    err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %" PRIu64,
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
        for (const auto& line : data) {
            for (const auto& stop : data[line.first]) {
                delete stop;
            }
            data[line.first].clear();
        }
        data.clear();

        cJSON *root = cJSON_Parse(local_response_buffer);
        cJSON *stop;
        cJSON_ArrayForEach(stop, root)
        {
            cJSON *ride;
            std::string stopKey(stop->string);
            ESP_LOGI(TAG, "Line key: %s", stopKey.c_str());
            data[stopKey] = std::vector<StopData*>();
            cJSON_ArrayForEach(ride, stop)
            {
                data[stopKey].push_back(new StopData(ride));
            }
            std::sort(data[stopKey].begin(), data[stopKey].end(), compareTime); 
            for (auto const& stop : data[stopKey]) {
                    stop->Print();
            }
        }
        cJSON_Delete(root);
        loaded = true;

        static uint32_t valueToSend = 0;
        ESP_LOGI(TAG, "Notifying main thread that data has been updated.");
        xTaskNotify(task, valueToSend, eSetValueWithoutOverwrite);
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    free(local_response_buffer);
    esp_http_client_cleanup(client);

    return err;
}

std::map<std::string, std::vector<StopData*>> *StopsData::GetData() {
    while (!loaded) {
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    return &data;
}

void StopsData::Loop(void *pvParameters) {
    StopsData *stopsData = (StopsData *)pvParameters;
    ESP_LOGI(TAG, "In StopsData::Loop()");
    stopsData->Load();
    int loop = 0;
    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        loop++;
        if (loop > DATA_LOAD_FREQUENCY) {
            stopsData->Load();
            loop = 0;
        }
    }
}

void StopsData::Start() {
    xTaskCreatePinnedToCore(Loop, "stops_data", 4096 * 2, (void *)this, 0, NULL, 1);
}
