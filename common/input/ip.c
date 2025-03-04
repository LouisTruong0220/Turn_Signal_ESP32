#include <stdio.h>
#include <esp_log.h>
#include "driver/gpio.h"
#include "rom/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "ip.h"

input_callback_t input_callback = NULL;
timeoutButoon_t timeout_Button = NULL;
static uint64_t _start, _stop;
static uint64_t _pressTick;
static TimerHandle_t xTimers;

static void gpio_input_handler(void* arg){
    int gpio_num = (uint32_t)arg;
    uint32_t rtc = xTaskGetTickCountFromISR();

    if(gpio_get_level(gpio_num) == 0){
        xTimerStart(xTimers, 0);
        _start = rtc;
    }
    else{
        xTimerStop(xTimers, 0);
        _stop = rtc;
        _pressTick = _stop - _start;
        input_callback(gpio_num, _pressTick);
    }
}

static void vTimerCallback( TimerHandle_t xTimer ){
    uint32_t ID;
    configASSERT( xTimer );
    ID = ( uint32_t ) pvTimerGetTimerID( xTimer );
    if(ID == 0){
        timeout_Button(BUTTON0);
        // printf("TIMEOUT\r\n");
    }
}

void input_io_create(gpio_num_t gpio_num, interrupt_type_edge level){
    gpio_pad_select_gpio(gpio_num);
    gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
    gpio_set_pull_mode(gpio_num, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(gpio_num, level);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio_num, gpio_input_handler, (void*)gpio_num);
    xTimers = xTimerCreate
                   ( /* Just a text name, not used by the RTOS kernel. */
                     "TimerForTimeout",
                     /* The timer period in ticks, must be greater than 0. */
                     5000/portTICK_PERIOD_MS,
                     /* The timers will auto-reload themselves when they expire. */
                     pdFALSE,
                     /* The ID is used to store a count of the number of times the
                        timer has expired, which is initialised to 0. */
                     ( void * ) 0,
                     /* Each timer calls the same callback when it expires. */
                     vTimerCallback
                   );
}

int input_io_get_level(gpio_num_t gpio_num){
    return gpio_get_level(gpio_num);
}

void input_set_callback(void* cb){
    input_callback = cb;
}

void input_set_timeout_callback(void* cb){
    timeout_Button = cb;
}