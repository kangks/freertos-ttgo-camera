/* TTGO OLED Camera example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"

#include "app_oled.h"

enum {
    PAGE_TIME = 0,
    PAGE_TEMP,
    PAGE_HUMINITY,
    PAGE_MAX,
};

enum {
    OLED_EVT_PAGE_NEXT,
    OLED_EVT_PAGE_PREV,
    OLED_EVT_SLEEP,
};

typedef struct {
    int type;
} oled_evt_t;

extern "C" void app_main();

QueueHandle_t oled_queue = NULL;
CI2CBus *i2c_bus_oled = NULL;
COled *oled = NULL;

static void oled_show_page(int page)
{
    static int page_prev = PAGE_MAX;
    if (page_prev == page) {

    } else {
        page_prev = page;
        oled->clean();
    }
    switch (page % PAGE_MAX) {
        case PAGE_TEMP:
//            oled->show_temp(hts221->read_temperature());
            break;
        case PAGE_HUMINITY:
//            oled->show_humidity(hts221->read_humidity());
            break;
        case PAGE_TIME:
            oled->show_time();
            break;
        default:
            oled->show_time();
            break;
    }
}

static void oled_task(void* arg)
{
    int cur_page = PAGE_TIME;
    portBASE_TYPE ret;
    oled_evt_t evt;
    oled_show_page(cur_page);
    while (1) {
        ret = xQueueReceive(oled_queue, &evt, 500 / portTICK_PERIOD_MS);
        if (ret == pdTRUE) {
            if (evt.type == OLED_EVT_PAGE_NEXT) {
                cur_page += (PAGE_MAX + 1);
                cur_page %= PAGE_MAX;
            } else if (evt.type == OLED_EVT_PAGE_PREV) {
                cur_page += (PAGE_MAX - 1);
                cur_page %= PAGE_MAX;
            } else if (evt.type == OLED_EVT_SLEEP) {
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                printf("enter deep-sleep\n");
            }
            oled_show_page(cur_page);
        } else {
            oled_show_page(cur_page);
        }
    }
}

static void i2c_dev_init()
{
    i2c_bus_oled = new CI2CBus((i2c_port_t) OLED_IIC_NUM, (gpio_num_t) OLED_IIC_SCL_NUM, (gpio_num_t) OLED_IIC_SDA_NUM, 100000);
    oled = new COled(i2c_bus_oled);
    oled->init();
}

static void application_init()
{
    // Init I2C devices
    i2c_dev_init();
}

void app_main()
{
    if (oled_queue == NULL) {
        oled_queue = xQueueCreate(10, sizeof(oled_evt_t));
    }

    application_init();
    xTaskCreate(oled_task, "oled_task", 1024 * 8, NULL, 12, NULL);
}
