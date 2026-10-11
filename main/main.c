#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "driver/gptimer.h"
#include "math.h"

#include "adc.h"
#include "pwm.h"
#include "encoder.h"
#include "pid.h"

#define PWM_PIN 6

#define ADC_MAX_VALUE 4095.0f
#define LOG_PERIOD_US 1e6
#define SETPOINT_STEP_SCALE 100

const static char *TAG = "LOG";

const EncoderConfig encoder_config = {
    .pin_a = 4,
    .pin_b = 5,
    .low_limit = -1000,
    .hight_limit = 1000};

const PidConfig pid_config = {
    .kp = 2.5f,
    .ki = 5.0f,
    .kd = 0.0f};

volatile float pid_setpoint = 3000.0f;

PwmHandle pwm;
AdcHandle adc;
PidHandle pid;
EncoderHandle encoder;

SemaphoreHandle_t pid_semaphore = NULL;

int64_t last_log_ts = 0;

static bool IRAM_ATTR on_timer_alarm(gptimer_handle_t timer,
                                     const gptimer_alarm_event_data_t *edata,
                                     void *user_ctx)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(pid_semaphore, &xHigherPriorityTaskWoken);
    return xHigherPriorityTaskWoken == pdTRUE;
}

static void setup_timer()
{
    gptimer_handle_t timer = NULL;
    gptimer_config_t cfg = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1 MHz → step 1 us
    };
    gptimer_new_timer(&cfg, &timer);

    gptimer_alarm_config_t alarm = {
        .alarm_count = 20000, // 20 000 us = 20 ms
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_set_alarm_action(timer, &alarm);

    gptimer_event_callbacks_t cbs = {.on_alarm = on_timer_alarm};
    gptimer_register_event_callbacks(timer, &cbs, NULL);

    gptimer_enable(timer);
    gptimer_start(timer);
}

static void setpoint_task(void *pvParameters)
{
    while (1)
    {
        int delta = encoder_get_delta(&encoder);
        if (delta != 0)
        {
            pid_setpoint += delta * SETPOINT_STEP_SCALE;
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

static void pid_task(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(pid_semaphore, portMAX_DELAY) == pdTRUE)
        {
            int pid_value = adc_read_value(&adc);
            float pid_output = pid_calc(&pid, pid_setpoint, (float)pid_value);

            float output = (pid_value + pid_output) / ADC_MAX_VALUE;
            if (output < 0.0f) output = 0.0f;
            if (output > 1.0f) output = 1.0f;

            pwm_set_duty(&pwm, output);

            const int64_t now_us = esp_timer_get_time();
            if (now_us - last_log_ts > LOG_PERIOD_US) {
                ESP_LOGI(TAG, "Setpoint: %.2f, Value: %.2f, Output: %.2f", pid_setpoint, (float)pid_value, output);
                last_log_ts = now_us;
            }
        }
    }
}

void app_main(void)
{
    pwm_setup_with_defaults(&pwm, PWM_PIN);
    adc_setup(&adc, ADC_DEFAULT_CHAN);
    encoder_setup(&encoder, &encoder_config);
    pid_setup(&pid, &pid_config);

    pid_semaphore = xSemaphoreCreateBinary();

    setup_timer();

    xTaskCreate(pid_task, "pid_task", 4096, NULL, 5, NULL);
    xTaskCreate(setpoint_task, "setpoint_task", 4096, NULL, 2, NULL);

    while (1)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}