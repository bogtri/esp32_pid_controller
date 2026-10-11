#pragma once

#include "driver/ledc.h"

#define DEFAULT_DUTY 0
#define DEFAULT_CHANNEL 0
#define DEFAULT_TIMER 0
#define DEFAULT_PWM_FREQUENCY 1000
#define DEFAULT_PWM_RESOLUTION 10

typedef struct
{
    uint32_t freq;
    uint32_t duty;
    uint8_t pin;
    uint8_t channel;
    uint8_t timer;
    uint8_t resolution;
} PwmConfig;

typedef struct
{
    uint32_t pwm_max_value;
    uint8_t channel;
} PwmHandle;

void pwm_setup(PwmHandle *handle, const PwmConfig *config);

void pwm_setup_with_defaults(PwmHandle *handle, const uint8_t pin);

void pwm_set_duty(PwmHandle *handle, const float value);