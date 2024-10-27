#include "m5epd_lvgl.h"
#include "data.hpp"

#ifndef __APPLICATION_H
#define __APPLICATION_H

#include "fonts.h"

#define RESTART_AFTER (8 * 3600)
#define REDRAW_AFTER  (15 * 60)
#define NEXT_LINES_AMOUNT 3

class OvstopsApplication;

class LineView {
    public:
        LineView(OvstopsApplication *app, lv_obj_t *parent);

        void UpdateData(std::vector<StopData*> stopData);

        ~LineView();

    protected:
        lv_obj_t *root;
        lv_obj_t *top;
        lv_obj_t *topLeft;
        lv_obj_t *topRight;
        lv_obj_t *status;

        lv_obj_t *lineContainer;
        lv_obj_t *lineLabel;
        lv_obj_t *destinationLabel;
        lv_obj_t *stopNameLabel;
        lv_obj_t *statusLabel;
        lv_obj_t *estimatedLabel;
        lv_obj_t *differenceLabel;

        lv_obj_t *next;

        std::vector<lv_obj_t*> nextRides;
        std::vector<lv_obj_t*> nextStatuses;
        std::vector<lv_obj_t*> nextEstimateds;
        std::vector<lv_obj_t*> nextDifferences;
};

class Application {
    public:
        Application() { 
        };
        ~Application() {
            if (screen != NULL) {
                lv_obj_del(screen);
            }
        };

        virtual bool init() { return false; };
        virtual bool setup() { return false; }
        virtual void run() { };
        
        void createScreen() {
             screen = lv_obj_create(NULL);
             errorScreen = lv_obj_create(NULL);
        };
        void show() {
            ::lv_scr_load(errorScreen);
        };
    protected:
        lv_obj_t *screen = NULL;
        lv_obj_t *errorScreen = NULL;


};

class OvstopsApplication : Application {
    public:
        bool init();
        bool setup();
        void run();

        lv_style_t defaultStyle;
        lv_style_t devStyle;
        lv_style_t lineNumberBoxStyle;
        lv_style_t lineNumberStyle;
        lv_style_t destinationStyle;
        lv_style_t stopNameStyle;
        lv_style_t bigStatusStyle;
        lv_style_t bigEstimatedStyle;
        lv_style_t bigDifferenceStyle;
        lv_style_t statusStyle;
        lv_style_t estimatedStyle;
        lv_style_t differenceStyle;

        lv_style_t errorStyle;

    protected:
        StopsData stopsData;

    private:
        std::map<std::string, LineView*> lines;

        void setupStyles();
        void updateData();
        void setupErrorScreen();

        lv_obj_t *rootContainer;
        lv_obj_t *errorContainer;
        lv_obj_t *errorLabel;
};
#endif