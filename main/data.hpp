#include <map>
#include <list>
#include <vector>
#include <string>
#include <stdbool.h>
#include <sys/param.h>
#include <math.h>
#include <algorithm>

#include "cJSON.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_crt_bundle.h"

#ifndef __DATA_H
#define __DATA_H

class WeatherData {
    public:
        WeatherData(cJSON *weather);
        ~WeatherData();

        void Print();

        char *GetTime() { return time; }
        char *GetDate() { return date; }
        // Returned in units of mm/h
        double GetPrecipitation() { return mmh; } 

        char *GetDescription() { return description; }
        char *GetRegion() { return region; }
        char *GetStationName() { return station; }
        double GetTemperature() { return temperature; }
        double GetFeelsLikeTemperature() { return feeltemperature; }
        double GetWindSpeed() { return windspeed; }

        char *GetFormattedTemperature() {
            if (temperature > 0.0) {
                snprintf(formattedTemperature, sizeof(formattedTemperature)-1, "+%.1f°", temperature);
            } else if (temperature < 0.0) {
                snprintf(formattedTemperature, sizeof(formattedTemperature)-1, "-%.1f°", temperature);
            } else {
                snprintf(formattedTemperature, sizeof(formattedTemperature)-1, "%.1f°", temperature);
            }
            return formattedTemperature;
        }

        char *GetFormattedWindspeed() {
            snprintf(formattedWindspeed, sizeof(formattedWindspeed)-1, "%.0f", windspeed);
            return formattedWindspeed;
        }

        char *GetFormattedMmh() {
            snprintf(formattedMmh, sizeof(formattedMmh)-1, "%.1f", mmh);
            return formattedMmh;
        }

    private:
        char *time;
        char *date;
        double mmh;

        char *description;
        char *region;
        char *station;

        char formattedTemperature[8];
        char formattedWindspeed[8];
        char formattedMmh[8];

        double temperature;
        double feeltemperature;
        double windspeed;      
};

class StopData {
    public:
        StopData(cJSON *stop);
        ~StopData();
        
        void Print();

        const char *GetLineNo() { return lineNo; }
        const char *GetLineName() { return lineName; }
        const char *GetDestination() { return destination; }
        const char *GetType() { return transportType; }
        const char *GetStopName() { return stopName; }
        const char *GetStatus() { return status; }
        const char *GetTimeTarget() {
            struct tm t;
            time_t tt = (time_t)(timeTarget/1000);
            localtime_r(&tt, &t);
            strftime(targetStr, sizeof(targetStr), "%H:%M", &t);
            return targetStr;
        }
        const char *GetTimeEstimated() {
            struct tm t;
            time_t tt = (time_t)(timeEstimated/1000);
            localtime_r(&tt, &t);
            strftime(estimatedStr, sizeof(estimatedStr), "%H:%M", &t);
            return estimatedStr;
        }
        const char *GetTimeDifference() {
            if (timeDifference == 0) {
                return "";
            }
            double d = ceil((double)((timeDifference/1000.0)/60.0));
            if (d > 0.0) {
                snprintf(differenceStr, sizeof(differenceStr), "+%.0f", d);
            } else {
                snprintf(differenceStr, sizeof(differenceStr), "%.0f", d);
            }
            return differenceStr;
        }

        int64_t CompareValue() {
            return timeDifference;
        }

    private:
        char *lineNo;
        char *lineName;
        char *destination;
        char *transportType;
        char *stopName;
        char *status;

        char targetStr[8] = { '\0' };
        char estimatedStr[8] = { '\0' };
        char differenceStr[8] = { '\0' };

        int64_t timeTarget = 0;
        int64_t timeEstimated = 0;
        int64_t timeDifference = 0;
};

class StopsData {
    public:
        StopsData() { };
        void SetUrl(const char *url);
        void Start();
        void SetNotifyTask(TaskHandle_t task_);

        std::map<std::string, std::vector<StopData*>> *GetData();
        virtual WeatherData *GetWeather() { return weather; };

    private:
        static void Loop(void *ptr);
        esp_err_t Load();
        static esp_err_t HttpEventHandler(esp_http_client_event_t *evt);

        const char *dataUrl;
        TaskHandle_t task;
        std::map<std::string, std::vector<StopData*>> data; 
        bool loaded = false;
        WeatherData *weather;

        static bool compareTime(StopData *s1, StopData *s2) 
        { 
            return (s1->CompareValue() > s2->CompareValue()); 
        } 

};

#endif
