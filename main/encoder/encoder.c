#include "encoder.h"

void encoder_setup(EncoderHandle *handle, const EncoderConfig *config)
{
    pcnt_unit_config_t unit_config = {
        .high_limit = config->hight_limit,
        .low_limit = config->low_limit,
    };
    pcnt_unit_handle_t pcnt_unit = NULL;
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = config->pin_a,
        .level_gpio_num = config->pin_b,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_HOLD, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));

    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));

    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

    handle->pcnt = pcnt_unit;
    handle->prev_value = 0;
}

int32_t encoder_get_value(EncoderHandle *handle)
{
    int pulse_count;
    ESP_ERROR_CHECK(pcnt_unit_get_count(handle->pcnt, &pulse_count));
    handle->prev_value = pulse_count;
    return pulse_count;
}

int32_t encoder_get_delta(EncoderHandle *handle) 
{
    int32_t prev = handle->prev_value;
    return encoder_get_value(handle) - prev;
}
