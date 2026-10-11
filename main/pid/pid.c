#include "pid.h"
#include "esp_timer.h"

static float get_seconds()
{
    return esp_timer_get_time() / 1e6f;
}

void pid_setup(PidHandle *handle, const PidConfig *config)
{
    handle->kp = config->kp;
    handle->ki = config->ki;
    handle->kd = config->kd;
    handle->i = 0;
    handle->prev_error = 0;
    handle->prev_ts = get_seconds();
}

float pid_calc(PidHandle *handle, volatile const float setpoint, const float value)
{
    float ts = get_seconds();
    float dt = ts - handle->prev_ts;
    handle->prev_ts = ts;

    float error = setpoint - value;

    handle->i += error * dt;
    if (handle->i < MIN_INTEGRAL)
        handle->i = MIN_INTEGRAL;
    if (handle->i > MAX_INTEGRAL)
        handle->i = MAX_INTEGRAL;

    float d = (error - handle->prev_error) / dt;
    handle->prev_error = error;

    float output = handle->kp * error + handle->ki * handle->i + handle->kd * d;

    return output;
}