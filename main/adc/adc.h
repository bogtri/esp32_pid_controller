#pragma once

#include "esp_adc/adc_oneshot.h"

#define ADC_SAMPLES 10

#define ADC_DEFAULT_CHAN ADC_CHANNEL_0
#define ADC_DEFAULT_ATTEN ADC_ATTEN_DB_12

typedef struct
{
    adc_oneshot_unit_handle_t adc;
    adc_channel_t channel;
} AdcHandle;

void adc_setup(AdcHandle *handle, const adc_channel_t channel);

uint32_t adc_read_value(AdcHandle *adc);