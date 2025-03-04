#include "led.h"

void led_set_timer(){
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .freq_hz = 4000,                      // frequency of PWM signal
        .speed_mode = LEDC_HIGH_SPEED_MODE,           // timer mode
        .timer_num = LEDC_TIMER_1,            // timer index
        .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
    };
    ledc_timer_config(&ledc_timer);
}

void led_set_channel(int gpio, int channel){
    ledc_channel_config_t ledc_channel = 
    {
        .channel    = channel,
        .duty       = 0,
        .gpio_num   = gpio,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER_1,
        .flags.output_invert = 0
    };

    ledc_channel_config(&ledc_channel);
}

void led_set_duty(int channel, uint16_t duty){
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, duty * 81);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);
}
