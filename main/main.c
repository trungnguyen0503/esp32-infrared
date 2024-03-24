#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const gpio_num_t LED4 = GPIO_NUM_12;
const gpio_num_t LED5 = GPIO_NUM_13;
const gpio_num_t ANALOG3 = GPIO_NUM_3;
const adc_channel_t ANALOG3_CHANNEL = ADC_CHANNEL_3;

const int ZERO_THRES = 450;
const int ONE_THRES = 4000;

const int LOOP_INTERVAL_MS = 20;

adc_oneshot_unit_handle_t adc1Handle;

int lastAnalogOutput = 0;
int count = 0;

void init(void) {
    ESP_ERROR_CHECK(gpio_set_direction(LED4, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(LED5, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_level(LED4, 1));
    ESP_ERROR_CHECK(gpio_set_level(LED5, 1));

    adc_oneshot_unit_init_cfg_t adc1InitCfg = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc1InitCfg, &adc1Handle));

    adc_oneshot_chan_cfg_t analog3Cfg = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1Handle, ANALOG3_CHANNEL, &analog3Cfg));

    ESP_ERROR_CHECK(adc_oneshot_read(adc1Handle, ANALOG3_CHANNEL, &lastAnalogOutput));
}

void loop(void) {
    int analogOutput = 0;

    adc_oneshot_read(adc1Handle, ANALOG3_CHANNEL, &analogOutput);
    if (analogOutput >= ONE_THRES && lastAnalogOutput <= ZERO_THRES) {
        count++;
        ESP_LOGI("LOG", "count = %d", count);
    }
    lastAnalogOutput = analogOutput;

    vTaskDelay(pdMS_TO_TICKS(LOOP_INTERVAL_MS));
}

void app_main(void) {
    init();
    while (1) {
        loop();
    }
}
