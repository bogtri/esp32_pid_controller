#pragma once

#define MIN_INTEGRAL -1000
#define MAX_INTEGRAL 1000

typedef struct
{
    float kp;
    float ki;
    float kd;
} PidConfig;

typedef struct
{
    float kp;
    float ki;
    float kd;
    float prev_ts;
    float prev_error;
    float i;
} PidHandle;

void pid_setup(PidHandle *handle, const PidConfig *config);

float pid_calc(PidHandle *handle, volatile const float setpoint, const float value);