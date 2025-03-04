#ifndef LED_H
#define LED_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/ledc.h"
#include "esp_err.h"

void led_set_timer();
void led_set_channel(int gpio, int channel);
void led_set_duty(int channel, uint16_t duty);

#endif