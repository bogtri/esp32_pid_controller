#include "adc.h"

void adc_setup(AdcHandle *handle, const adc_channel_t channel)
{
    adc_oneshot_unit_handle_t adc_handle;

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_DEFAULT_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, channel, &config));

    handle->adc = adc_handle;
    handle->channel = channel;
}

uint32_t adc_read_value(AdcHandle *handle)
{
    int adc_avg = 0, adc_raw = 0;
    for (int i = 0; i < ADC_SAMPLES; i++)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(handle->adc, handle->channel, &adc_raw));
        adc_avg += adc_raw;
    }
    return adc_avg / ADC_SAMPLES;
}