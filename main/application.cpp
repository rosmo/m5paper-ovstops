#include "application.hpp"

#define TAG "Application"

bool OvstopsApplication::init() {
    return true;
}

void OvstopsApplication::setupStyles() {
    std::list<lv_style_t*> allStyles = {
        &defaultStyle,
        &devStyle,
        &lineNumberStyle,
        &lineNumberBoxStyle,
        &destinationStyle,
        &stopNameStyle,
        &bigStatusStyle,
        &bigEstimatedStyle,
        &bigDifferenceStyle,
        &statusStyle,
        &estimatedStyle,
        &differenceStyle,
        &errorStyle,
    };
    for (lv_style_t *st : allStyles) {
        lv_style_init(st);
        lv_style_set_bg_color(st, lv_color_white());
        lv_style_set_text_color(st, lv_color_black());
        lv_style_set_text_font(st, APP_FONT(40));
        
        // Add borders for layout debugging
        lv_style_set_border_color(st, lv_color_black());
    }
    
    lv_style_set_border_width(&devStyle, 3);
    lv_style_set_text_font(&devStyle, APP_FONT(24));

    lv_color_t grey = lv_color_make(222, 222, 222);

    lv_style_set_bg_color(&weatherContainerStyle, lv_color_white());
    lv_style_set_bg_opa(&weatherContainerStyle, LV_OPA_100);

    lv_style_set_bg_color(&rainBoxStyle, grey);
    lv_style_set_bg_opa(&rainBoxStyle, LV_OPA_100);
    lv_style_set_bg_color(&windBoxStyle, grey);
    lv_style_set_bg_opa(&windBoxStyle, LV_OPA_100);
    lv_style_set_bg_color(&tempBoxStyle, grey);
    lv_style_set_bg_opa(&tempBoxStyle, LV_OPA_100);
    lv_style_set_bg_color(&infoBoxStyle, grey);
    lv_style_set_bg_opa(&infoBoxStyle, LV_OPA_100);

    lv_style_set_text_font(&rainStyle, APP_FONT_BOLD(42));
    lv_style_set_text_color(&rainStyle, lv_color_black());
    lv_style_set_text_font(&windStyle, APP_FONT_BOLD(42));
    lv_style_set_text_color(&windStyle, lv_color_black());
    lv_style_set_text_font(&tempStyle, APP_FONT_BOLD(42));
    lv_style_set_text_color(&tempStyle, lv_color_black());

    lv_style_set_text_font(&rainLegendStyle, APP_FONT(24));
    lv_style_set_text_color(&rainLegendStyle, lv_color_black());
    lv_style_set_text_font(&windLegendStyle, APP_FONT(24));
    lv_style_set_text_color(&windLegendStyle, lv_color_black());
    lv_style_set_text_font(&tempLegendStyle, APP_FONT(24));
    lv_style_set_text_color(&tempLegendStyle, lv_color_black());
    
    lv_style_set_text_font(&stationNameStyle, APP_FONT_BOLD(32));
    lv_style_set_text_color(&stationNameStyle, lv_color_black());
    lv_style_set_text_font(&descriptionStyle, APP_FONT(24));
    lv_style_set_text_color(&descriptionStyle, lv_color_black());

    lv_style_set_text_font(&lineNumberStyle, APP_FONT_BOLD(96));
    lv_style_set_border_width(&lineNumberStyle, 2);
    lv_style_set_text_color(&lineNumberStyle, lv_color_white());
    lv_style_set_text_font(&destinationStyle, APP_FONT_BOLD(42));
    lv_style_set_text_font(&stopNameStyle, APP_FONT(36));

    lv_style_set_bg_color(&lineNumberBoxStyle, lv_color_black());
    lv_style_set_bg_opa(&lineNumberBoxStyle, LV_OPA_100);

    lv_style_set_text_font(&bigStatusStyle, APP_FONT(40));
    lv_style_set_text_font(&bigEstimatedStyle, APP_FONT(40));
    lv_style_set_text_font(&bigDifferenceStyle, APP_FONT_BOLD(40));

    lv_style_set_text_font(&statusStyle, APP_FONT(32));
    lv_style_set_text_font(&estimatedStyle, APP_FONT(32));
    lv_style_set_text_font(&differenceStyle, APP_FONT_BOLD(32));

    ESP_LOGI(TAG, "Styles set up!");
}

LineView::LineView(OvstopsApplication *app, lv_obj_t *parent) {    
    root = lv_obj_create(parent);
    lv_obj_set_size(root, LV_PCT(100), LV_PCT(30));
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
    // lv_obj_set_flex_align(root, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_AROUND);
    lv_obj_set_style_pad_left(root, 5, LV_PART_MAIN);
    lv_obj_set_style_pad_right(root, 5, LV_PART_MAIN);
    lv_obj_set_style_pad_top(root, 5, LV_PART_MAIN);

    top = lv_obj_create(root);
    lv_obj_set_size(top, LV_PCT(100), 135);
    lv_obj_set_flex_flow(top, LV_FLEX_FLOW_ROW);
    lv_obj_add_style(top, &app->defaultStyle, 0);

    lineContainer = lv_obj_create(top);
    lv_obj_add_style(lineContainer, &app->lineNumberBoxStyle, 0);

    lineLabel = lv_label_create(lineContainer);
    lv_label_set_text(lineLabel, "5");
    lv_obj_align(lineLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(lineLabel, &app->lineNumberStyle, 0);
    lv_obj_set_size(lineLabel, 140, 130);
    lv_obj_set_style_text_align(lineLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    topRight = lv_obj_create(top);
    lv_obj_set_flex_flow(topRight, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_left(topRight, 5, LV_PART_MAIN);

    // Allow the destination container to grow
    lv_obj_set_flex_grow(topRight, 1);

    destinationLabel = lv_label_create(topRight);
    lv_label_set_text(destinationLabel, "Amstelveen Stadshart");
    lv_obj_align(destinationLabel, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_style(destinationLabel, &app->destinationStyle, 0);

    stopNameLabel = lv_label_create(topRight);
    lv_label_set_text(stopNameLabel, "Bloemgracht");
    lv_obj_align(stopNameLabel, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_style(stopNameLabel, &app->stopNameStyle, 0);

    status = lv_obj_create(topRight);
    lv_obj_set_width(status, LV_PCT(100));
    lv_obj_set_flex_flow(status, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_grow(status, 1);

    statusLabel = lv_label_create(status);
    lv_label_set_text(statusLabel, "DRIVING");
    // lv_obj_align(statusLabel, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_style(statusLabel, &app->bigStatusStyle, 0);
    lv_obj_set_flex_grow(statusLabel, 1);

    estimatedLabel = lv_label_create(status);
    lv_label_set_text(estimatedLabel, "12:35");
    lv_obj_set_style_text_align(estimatedLabel, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
    lv_obj_align(estimatedLabel, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_style(estimatedLabel, &app->bigEstimatedStyle, 0);
    lv_obj_set_width(estimatedLabel, 100);
    
    differenceLabel = lv_label_create(status);
    lv_obj_set_width(differenceLabel, 100);
    lv_label_set_text(differenceLabel, "+2");
    lv_obj_set_style_text_align(differenceLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(differenceLabel, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_style(differenceLabel, &app->bigDifferenceStyle, 0);

    // Next 3 (or more) rides
    next = lv_obj_create(root);
    lv_obj_set_width(next, LV_PCT(100));
    lv_obj_set_flex_flow(next, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(root, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_AROUND);
    lv_obj_set_flex_grow(next, 1);

    // Initialize vectors
    nextRides = decltype(nextRides)(NEXT_LINES_AMOUNT);
    nextStatuses = decltype(nextStatuses)(NEXT_LINES_AMOUNT);
    nextEstimateds = decltype(nextEstimateds)(NEXT_LINES_AMOUNT);
    nextDifferences = decltype(nextDifferences)(NEXT_LINES_AMOUNT);
    
    for (int i = 0; i < NEXT_LINES_AMOUNT; i++) {
        nextRides[i] = lv_obj_create(next);
        lv_obj_set_size(nextRides[i], LV_PCT(100), 45);
        lv_obj_set_flex_flow(nextRides[i], LV_FLEX_FLOW_ROW);
    
        nextStatuses[i] = lv_label_create(nextRides[i]);
        lv_label_set_text(nextStatuses[i], "PLANNED");
        lv_obj_add_style(nextStatuses[i], &app->statusStyle, 0);
        lv_obj_set_flex_grow(nextStatuses[i], 1);
        lv_obj_set_style_pad_left(nextStatuses[i], 10, LV_PART_MAIN);

        nextEstimateds[i] = lv_label_create(nextRides[i]);
        lv_label_set_text(nextEstimateds[i], "12:52");
        lv_obj_set_style_text_align(nextEstimateds[i], LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
        lv_obj_align(nextEstimateds[i], LV_ALIGN_RIGHT_MID, 0, 0);
        lv_obj_add_style(nextEstimateds[i], &app->estimatedStyle, 0);
        lv_obj_set_width(nextEstimateds[i], 100);

        nextDifferences[i] = lv_label_create(nextRides[i]);
        lv_obj_set_width(nextDifferences[i], 100);
        lv_label_set_text(nextDifferences[i], "+1");
        lv_obj_set_style_text_align(nextDifferences[i], LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(nextDifferences[i], LV_ALIGN_RIGHT_MID, 0, 0);
        lv_obj_add_style(nextDifferences[i], &app->differenceStyle, 0);
    }
}

void LineView::UpdateData(std::vector<StopData*> stopData) {
    ESP_LOGI(TAG, "Data update start: %d lines", stopData.size());
    if (stopData.size() >= 1) {
        StopData *stop = stopData.at(0);
        lv_label_set_text(lineLabel, stop->GetLineNo());
        lv_label_set_text(destinationLabel, stop->GetDestination());
        lv_label_set_text(stopNameLabel, stop->GetStopName());
        lv_label_set_text(statusLabel, stop->GetStatus());
        lv_label_set_text(estimatedLabel, stop->GetTimeEstimated());
        lv_label_set_text(differenceLabel, stop->GetTimeDifference());
    }
    if (stopData.size() > 1) {
        int len = (stopData.size() - 1) > NEXT_LINES_AMOUNT ? NEXT_LINES_AMOUNT : stopData.size() - 1;
        for (int i = 1; i < (len + 1); i++) {
            StopData *stop = stopData.at(i);
            lv_label_set_text(nextStatuses[i - 1], stop->GetStatus());
            lv_label_set_text(nextEstimateds[i - 1], stop->GetTimeEstimated());
            lv_label_set_text(nextDifferences[i - 1], stop->GetTimeDifference());
        }
    }
    ESP_LOGI(TAG, "Data updated.");
}

LineView::~LineView() {
    ESP_LOGI(TAG, "LineView destructor");
}

WeatherView::WeatherView(OvstopsApplication *app, lv_obj_t *parent) {    
    root = lv_obj_create(parent);
    lv_obj_set_size(root, LV_PCT(100), 110);
    lv_obj_set_style_pad_left(root, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_right(root, 5, LV_PART_MAIN);
    //lv_obj_set_style_pad_top(root, 5, LV_PART_MAIN);
    // lv_obj_set_style_pad_bottom(root, 10, LV_PART_MAIN);

    weatherContainer = lv_obj_create(root);
    lv_obj_set_flex_flow(weatherContainer, LV_FLEX_FLOW_ROW);
    lv_obj_set_size(weatherContainer, LV_PCT(100), LV_PCT(100));
    lv_obj_add_style(weatherContainer, &app->weatherContainerStyle, 0);
    // lv_obj_add_style(weatherContainer, &app->devStyle, 0);

    tempBox = lv_obj_create(weatherContainer);
    rainBox = lv_obj_create(weatherContainer);
    windBox = lv_obj_create(weatherContainer);
    std::map<lv_obj_t*, lv_style_t*> boxes = {
        { tempBox, &app->rainBoxStyle },
        { rainBox, &app->tempBoxStyle },
        { windBox, &app->windBoxStyle },
    };
    for (auto const& o : boxes) {
        lv_obj_add_style(o.first, const_cast<lv_style_t*>(o.second), 0);
        lv_obj_set_flex_flow(o.first, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(o.first, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_size(o.first, LV_SIZE_CONTENT, 100);
        lv_obj_set_style_pad_left(o.first, 10, LV_PART_MAIN);
    }
    lv_obj_set_style_pad_right(windBox, 10, LV_PART_MAIN);

    rainLabel = lv_label_create(rainBox);
    lv_label_set_text(rainLabel, "2.7");
    windLabel = lv_label_create(windBox);
    lv_label_set_text(windLabel, "48");
    tempLabel = lv_label_create(tempBox);
    lv_label_set_text(tempLabel, "+5°");

    std::map<lv_obj_t*, lv_style_t*> labels = {
        { rainLabel, &app->rainStyle },
        { tempLabel, &app->tempStyle },
        { windLabel, &app->windStyle },
    };
    for (auto const& o : labels) {
        lv_obj_align(o.first, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_style(o.first, const_cast<lv_style_t*>(o.second), 0);
        lv_obj_set_size(o.first, LV_SIZE_CONTENT, 50);
        lv_obj_set_style_text_align(o.first, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    }

    tempSubLabel = lv_label_create(tempBox);
    lv_label_set_text(tempSubLabel, "celsius");
    rainSubLabel = lv_label_create(rainBox);
    lv_label_set_text(rainSubLabel, "mm/h");
    windSubLabel = lv_label_create(windBox);
    lv_label_set_text(windSubLabel, "m/s");

    std::map<lv_obj_t*, lv_style_t*> subLabels = {
        { rainSubLabel, &app->rainLegendStyle },
        { tempSubLabel, &app->tempLegendStyle },
        { windSubLabel, &app->windLegendStyle },
    };
    for (auto const& o : subLabels) {
        lv_obj_align(o.first, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_style(o.first, const_cast<lv_style_t*>(o.second), 0);
        lv_obj_set_size(o.first, LV_SIZE_CONTENT, 30);
        lv_obj_set_style_text_align(o.first, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    }

    infoBox = lv_obj_create(weatherContainer);
    lv_obj_add_style(infoBox, &app->infoBoxStyle, 0);
    lv_obj_set_style_pad_left(infoBox, 10, LV_PART_MAIN);
    lv_obj_set_flex_flow(infoBox, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(infoBox, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_size(infoBox, LV_PCT(100), 100);

    stationLabel = lv_label_create(infoBox);
    lv_label_set_text(stationLabel, "Amsterdam");
    lv_label_set_long_mode(stationLabel, LV_LABEL_LONG_DOT);
    lv_obj_align(stationLabel, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_style(stationLabel, &app->stationNameStyle, 0);
    lv_obj_set_style_text_align(stationLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);

    descriptionLabel = lv_label_create(infoBox);
    lv_label_set_text(descriptionLabel, "Huge firestorms");
    lv_label_set_long_mode(descriptionLabel, LV_LABEL_LONG_DOT);
    lv_obj_align(descriptionLabel, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_style(descriptionLabel, &app->descriptionStyle, 0);
    lv_obj_set_style_text_align(descriptionLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
}

void WeatherView::UpdateWeather(WeatherData *weatherData) {
    ESP_LOGI(TAG, "Updating weather data");
    lv_label_set_text(tempLabel, weatherData->GetFormattedTemperature());
    lv_label_set_text(windLabel, weatherData->GetFormattedWindspeed());
    lv_label_set_text(rainLabel, weatherData->GetFormattedMmh());
    lv_label_set_text(stationLabel, weatherData->GetStationName());
    lv_label_set_text(descriptionLabel, weatherData->GetDescription());
}

WeatherView::~WeatherView() {
    ESP_LOGI(TAG, "WeatherView destructor");
}

void OvstopsApplication::updateData() {
    WeatherData *weatherData = stopsData.GetWeather();
    if (weatherData != NULL) {
        ESP_LOGI(TAG, "Updating weather data...");
        weather->UpdateWeather(weatherData);
    }
    std::map<std::string, std::vector<StopData*>> *linesData = stopsData.GetData();
    if (linesData == NULL) {
        ESP_LOGW(TAG, "Line data not loaded...");
        return;
    }
    for (const auto& [lineId, stops] : *linesData) {
        LineView *lineView;
        if (auto search = lines.find(lineId); search != lines.end()) {
            ESP_LOGI(TAG, "Found existing line: %s", lineId.c_str());
            lineView = lines[lineId];
        } else {
            ESP_LOGI(TAG, "Creating new line: %s", lineId.c_str());
            lineView = new LineView(this, rootContainer);
            lines[lineId] = lineView;
        }
        lineView->UpdateData(stops);
    }
}

void OvstopsApplication::setupErrorScreen() {
    errorContainer = lv_obj_create(errorScreen);
    lv_obj_align(errorContainer, LV_ALIGN_CENTER, 0, 100);
    lv_obj_set_size(errorContainer, LV_PCT(100), LV_PCT(100));
    // lv_obj_set_flex_flow(errorContainer, LV_FLEX_FLOW_COLUMN);

    errorLabel = lv_label_create(errorContainer);
    lv_label_set_text(errorLabel, "Loading data...");
    lv_obj_align(errorLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(errorLabel, &errorStyle, 0);
    lv_obj_set_style_text_align(errorLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_label_set_long_mode(errorLabel, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(errorLabel, LV_PCT(100));
}

bool OvstopsApplication::setup() {    
    createScreen();
    setupStyles();
    setupErrorScreen();

    rootContainer = lv_obj_create(screen);
    lv_obj_align(rootContainer, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_size(rootContainer, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(rootContainer, LV_FLEX_FLOW_COLUMN);
    // lv_obj_set_flex_align(rootContainer, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_AROUND);
    
    weather = new WeatherView(this, rootContainer);

    return true;
}

void OvstopsApplication::run() {
    void *receivedValue;
    TickType_t startTicks, currentTicks;
    TickType_t redrawStartTicks;

    startTicks = xTaskGetTickCount();
    redrawStartTicks = xTaskGetTickCount();
    show();

    stopsData.SetUrl(CONFIG_OVS_WORKER_URL);
    stopsData.SetNotifyTask(xTaskGetCurrentTaskHandle());
    stopsData.Start();
    updateData();

    ESP_LOGI(TAG, "Starting run loop...");
    while (true) {
        vTaskDelay(50 / portTICK_PERIOD_MS);
        LVGL.timer(50);
        // Wait for updated data
        if (xTaskNotifyWait(0, ULONG_MAX, (uint32_t *)&receivedValue, 50 / portTICK_PERIOD_MS) == pdPASS) {
            ESP_LOGI(TAG, "Received notification from HTTP task...");
            if (receivedValue == NULL) {
                ESP_LOGI(TAG, "New data received, updating...");
                updateData();
                ::lv_scr_load(screen);
            } else {
                ESP_LOGI(TAG, "Error received: %s", (char *)receivedValue);
                ::lv_scr_load(errorScreen);
                lv_label_set_text(errorLabel, (char *)receivedValue);
            }
        }
        currentTicks = xTaskGetTickCount();
        if (((currentTicks - startTicks) * portTICK_PERIOD_MS) >= (RESTART_AFTER * 1000)) {
            ESP_LOGW(TAG, "Restarting device after %d seconds.", RESTART_AFTER);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            esp_restart();
        }
        if (((currentTicks - redrawStartTicks) * portTICK_PERIOD_MS) >= (REDRAW_AFTER * 1000)) {
            
            ESP_LOGW(TAG, "Refreshing screen after %d seconds.", REDRAW_AFTER);
            LVGL.refresh();
            lv_obj_invalidate(lv_scr_act());
            redrawStartTicks = currentTicks;
        }
    }
}