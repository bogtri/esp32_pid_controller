idf_component_register(SRC_DIRS "." "pwm" "adc" "pid" "encoder"
                    PRIV_REQUIRES esp_driver_gpio esp_driver_ledc esp_timer esp_driver_pcnt esp_adc esp_driver_gptimer
                    INCLUDE_DIRS "." "pwm" "adc" "encoder" "pid")
