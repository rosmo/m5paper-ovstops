#include "m5epd_lvgl.h"

bool M5_LVGL::initialized = false;
lgfx::Panel_IT8951 *p = new lgfx::Panel_IT8951();
lgfx::Touch_GT911 *t = new lgfx::Touch_GT911();

void M5_LVGL::refresh() {
    if (!gfx->init()) {
        printf("LovyanGFX initialization failed!\n");
        return;
    }
}

void displayFlush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) {
    LGFX *_gfx = (LGFX *)disp->user_data;

    const u_long w = area->x2 - area->x1 + 1;
    const u_long h = area->y2 - area->y1 + 1;
    _gfx->startWrite();
    _gfx->setAddrWindow(area->x1, area->y1, w, h);

    uint8_t *buf = static_cast<uint8_t *>(&color_p->full);
    uint16_t *newBuf = (uint16_t *)calloc(w * h, sizeof(uint16_t));
    for (int i = 0; i < w * h; i++) {
        newBuf[i] = (((buf[i] & 0b11111000) << 8) + ((buf[i] & 0b11111100) << 3) + (buf[i] >> 3));
    }
    _gfx->writePixels(newBuf, w * h, true);
    free(newBuf);
    _gfx->endWrite();

    ::lv_disp_flush_ready(disp);
}


void touchInput(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    LGFX *_gfx = (LGFX *)drv->user_data;
    lgfx::touch_point_t tp;
    uint8_t count;
    static int32_t x, y;
    
    count = _gfx->getTouch(&tp);
    if (count > 0)
    {
        // printf("Touch screen results: count=%d, tx=%d, ty=%d, id=%d, size=%d\n", count, tp.x, tp.y, tp.id, tp.size);
        data->point.x = tp.x;
        data->point.y = tp.y;
        data->state = LV_INDEV_STATE_PR;
        x = tp.x;
        y = tp.y;
    } else {
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_REL;
    }
}
void lvglTimerTask(void *pvParameters) {
    while (true) {
        ::lv_tick_inc(20);
        ::lv_task_handler();
        ::lv_timer_handler();
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
} 

void M5_LVGL::timer(int ticks) {
    ::lv_tick_inc(ticks);
    ::lv_task_handler();
    ::lv_timer_handler();
}

//void vApplicationTickHook() {
//    ::lv_tick_inc(1000 / CONFIG_FREERTOS_HZ);
//}

bool M5_LVGL::initialize(int rotation) {
    gfx = new LGFX();
    if (!gfx->init()) {
        printf("LovyanGFX initialization failed!\n");
        return false;
    }
    gfx->touch()->wakeup();
    gfx->setBrightness(128);

    gfx->setEpdMode(epd_mode_t::epd_fast);  
    
    // gfx->setRotation(rotation);

    vTaskDelay(100 / portTICK_PERIOD_MS);

    ::lv_init();
    ::lv_disp_draw_buf_init(&drawBuffer, buf, secondBuf, M5EPD_PANEL_H * 20);

    // Initial screen
    ::lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = M5EPD_PANEL_H;
    disp_drv.ver_res = M5EPD_PANEL_W;
    disp_drv.flush_cb = displayFlush;
    disp_drv.draw_buf = &drawBuffer;
    disp_drv.user_data = (void *)gfx; 
    ::lv_disp_drv_register(&disp_drv);

    // Initialize touch screen input
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchInput;
    indev_drv.user_data = (void *)gfx;
    ::lv_indev_drv_register(&indev_drv);

    // xTaskCreatePinnedToCore(lvglTimerTask, "lvgl", 4096, NULL, 1, NULL, 1);

    initialized = true;
    return initialized;
}

M5_LVGL LVGL;