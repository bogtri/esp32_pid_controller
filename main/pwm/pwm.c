#include "pwm.h"

void pwm_setup(PwmHandle *handle, const PwmConfig *config)
{
    const ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = config->resolution,
        .timer_num = config->timer,
        .freq_hz = config->freq,
        .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    const ledc_channel_config_t channel_config = {
        .gpio_num = config->pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = config->channel,
        .timer_sel = config->timer,
        .duty = config->duty,
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&channel_config));

    handle->pwm_max_value = (1 << config->resolution) - 1;
    handle->channel = config->channel;
}

void pwm_setup_with_defaults(PwmHandle *handle, const uint8_t pin)
{
    const PwmConfig pwm_config = {
        .pin = pin,
        .duty = DEFAULT_DUTY,
        .timer = DEFAULT_TIMER,
        .channel = DEFAULT_CHANNEL,
        .freq = DEFAULT_PWM_FREQUENCY,
        .resolution = DEFAULT_PWM_RESOLUTION};

    pwm_setup(handle, &pwm_config);
}

/*
    Converts percentage value to PWM duty and outputs it
*/
void pwm_set_duty(PwmHandle *handle, float value)
{
    if (value >= 0 && value <= handle->pwm_max_value) {
       uint32_t duty = value * handle->pwm_max_value;
        ledc_set_duty(LEDC_LOW_SPEED_MODE, handle->channel, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, handle->channel);
    }
}