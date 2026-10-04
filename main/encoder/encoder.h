#pragma once

#include "driver/pulse_cnt.h"

typedef struct
{
    int pin_a;
    int pin_b;
    int low_limit;
    int hight_limit;
} EncoderConfig;

typedef struct
{
    pcnt_unit_handle_t pcnt;
    int32_t prev_value;
} EncoderHandle;

void encoder_setup(EncoderHandle *handle, const EncoderConfig *config);

int32_t encoder_get_value(EncoderHandle *handle);
int32_t encoder_get_delta(EncoderHandle *handle);