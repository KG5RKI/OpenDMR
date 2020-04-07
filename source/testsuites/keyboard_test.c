
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "stm32f4xx.h"
#include "keyboard.h"
#include "gpio.h"
#include "usb_vcp.h"

static void sleep(uint32_t);

static void green_main(void*);
static void fw_main_task(void*);

typedef enum { NO_EVENT = 0, KEY_EVENT = 0x01, BUTTON_EVENT = 0x02, FUNCTION_EVENT = 0x04, ROTARY_EVENT = 0x08 } uiEventInput_t;

int main (void)
{
    gpio_setMode(GPIOE, 0, OUTPUT);
    gpio_setMode(GPIOE, 1, OUTPUT);
    gpio_setMode(GPIOD, 12, OUTPUT);
    gpio_setMode(GPIOD, 13, OUTPUT);

    fw_init_keyboard();
    init_pit();

    // Reset the display
    gpio_clearPin(GPIOD, 13);
    gpio_clearPin(GPIOD, 12);
    xTaskCreate(green_main, "grn", 256, NULL, 1, NULL);
    xTaskCreate(fw_main_task, "main", 256, NULL, 2, NULL);
    vTaskStartScheduler();

    for(;;);
}

static void fw_main_task(void* data) {
	keyboardCode_t keys;
	int key_event;

    printf("Keyboard test initialized!\r\n");
    for(;;) {
        fw_check_key_event(&keys, &key_event); // Read keyboard state and event
        if (key_event != NO_EVENT) {
            if(KEYCHECK_UP(keys, keys.key))
                printf("%c key up!\r\n", keys.key);
            else if (KEYCHECK_SHORTUP(keys, keys.key))
                printf("%c key shortup!\r\n", keys.key);
            if (KEYCHECK_DOWN(keys, keys.key))
                printf("%c key down!\r\n", keys.key);
            if (KEYCHECK_PRESS(keys, keys.key))
                printf("%c key press!\r\n", keys.key);
            else if (KEYCHECK_LONGDOWN(keys, keys.key))
                printf("%c key long press!\r\n", keys.key);
        }
        vTaskDelay(1);
    }
}

static void green_main(void* machtnichts) {

    for(;;)
    {
        GPIOE->ODR ^= (1 << 0); // PE0
        sleep(1000);
    }
}

static void sleep(uint32_t ms)
{
    vTaskDelay(ms);
}

void vApplicationTickHook() { }
void vApplicationStackOverflowHook() { }
void vApplicationIdleHook() { }
void vApplicationMallocFailedHook() { }
