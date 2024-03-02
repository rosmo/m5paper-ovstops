#include <map>
#include <list>
#include <vector>
#include <string>
#include <stdbool.h>
#include <sys/param.h>
#include <math.h>
#include "cJSON.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_crt_bundle.h"

#ifndef __DATA_H
#define __DATA_H

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
        
    private:
        static void Loop(void *ptr);
        esp_err_t Load();
        static esp_err_t HttpEventHandler(esp_http_client_event_t *evt);

        const char *dataUrl;
        TaskHandle_t task;
        std::map<std::string, std::vector<StopData*>> data; 
        bool loaded = false;

        static bool compareTime(StopData *s1, StopData *s2) 
        { 
            return (s1->CompareValue() > s2->CompareValue()); 
        } 

};

#endif
