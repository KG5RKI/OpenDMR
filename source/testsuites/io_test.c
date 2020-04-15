/***************************************************************************
 *   Copyright (C) 2020 by Niccolò Izzo IU2KIN                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "stm32f4xx.h"
#include "keyboard.h"
#include "buttons.h"
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

	fw_init_buttons();
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
	uint32_t buttons;
	int button_event;
	keyboardCode_t keys;
	int key_event;

    printf("Keyboard test initialized!\r\n");
    for(;;) {
	    fw_check_button_event(&buttons, &button_event);// Read button state and event
        fw_check_key_event(&keys, &key_event); // Read keyboard state and event
        if (button_event != NO_EVENT) {
            if (buttons & BUTTON_PTT)
                printf("PTT was pressed!\r\n");
            if (buttons & BUTTON_SK1)
                printf("SK1 was pressed!\r\n");
            if (buttons & BUTTON_SK2)
                printf("SK2 was pressed!\r\n");
        }
        if (key_event != NO_EVENT) {
            // Give printable shape to special symbols
            if (keys.key == 13)
                keys.key = 'G';
            if (keys.key == 27)
                keys.key = 'R';
            if (keys.key == 1)
                keys.key = 'U';
            if (keys.key == 2)
                keys.key = 'D';
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
