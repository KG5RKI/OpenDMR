/***************************************************************************
 *   Copyright (C) 2020 by Silvano Seva IU2KWO and Niccolò Izzo IU2KIN     *
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
#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "keyboard.h"
#include "menuSystem.h"
#include "pit.h"
#include "adc1.h"
#include "lcd.h"
#include "gpio.h"
#include "keyboard.h"
#include "buttons.h"
#include "stm32f4xx.h"
#include "ticks.h"
#include "rotary_switch.h"

// 120 secs = Sample window * BATTERY_VOLTAGE_TICK_RELOAD in milliseconds
#define AVERAGE_BATTERY_VOLTAGE_SAMPLE_WINDOW 60.0f

float averageBatteryVoltage;

static void sleep(uint32_t);

static void main_task(void*);
static void led_task(void*);
static void battery_average(void*);

int main (void)
{

    gpio_setMode(GPIOE, 0, OUTPUT);
    gpio_setMode(GPIOE, 1, OUTPUT);

//    printf("Welcome to OpenDMR v0.1 compiled for the MD380\n\r");
//    printf("             Viva il DMR libero!              \n\r");

#ifdef LIGHT_THEME
    graphicsInit(COLOR_WHITE, COLOR_BLACK, COLOR_BLACK);
#elif DARK_THEME
    graphicsInit(COLOR_BLACK, COLOR_WHITE, COLOR_WHITE);
#else
    graphicsInit(COLOR_WHITE, COLOR_BLACK, COLOR_BLACK);
#endif
    lcd_setBacklightLevel(254);

    // Initialize everything
    adc1_init();
    fw_init_keyboard();
    init_pit();

    xTaskCreate(led_task, "green_task", 256, NULL, 1, NULL);
    xTaskCreate(main_task, "main_task", 256, NULL, 1, NULL);
    xTaskCreate(battery_average, "bat_avg_task", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    /* This point is never reached */
    for(;;) ;
}

static void main_task(void* machtnichts) {
	keyboardCode_t keys;
	int key_event;
	int keyFunction;
	uint32_t buttons;
	int button_event;
	uint32_t rotary;
	int rotary_event;
	int function_event;
	uiEvent_t ev = { .buttons = 0, .keys = NO_KEYCODE, .rotary = 0, .function = 0, .events = NO_EVENT, .hasEvent = false, .time = 0 };
	bool keyOrButtonChanged = false;

	menuBatteryInit(); // Initialize circular buffer
    menuInitMenuSystem();

    // Call battery menu
	menuSystemPushNewMenu(MENU_BATTERY);

    for(;;)
    {
        // Collect input events and pass them to the menu
	    fw_check_button_event(&buttons, &button_event);
		fw_check_key_event(&keys, &key_event); // Read keyboard state and event
		check_rotary_switch_event(&rotary, &rotary_event); // Rotary switch state and event (GD-77S only)
		function_event = NO_EVENT;
		keyOrButtonChanged = ((key_event != NO_EVENT) || (button_event != NO_EVENT) || (rotary_event != NO_EVENT));
        ev.buttons = buttons;
        ev.keys = keys;
        ev.rotary = rotary;
        ev.events = function_event | (button_event << 1) | (rotary_event << 3) | key_event;
        ev.hasEvent = keyOrButtonChanged || function_event;
        ev.time = fw_millis();

        menuSystemCallCurrentMenuTick(&ev);
    }
}

static void led_task(void* machtnichts) {
    for(;;)
    {
        gpio_togglePin(GPIOE, 0);
        vTaskDelay(500);
    }
}

static void battery_average(void* machtnichts) {
    static int battery_voltage = 0;
    static bool batteryVoltageHasChanged = false;
    static int battery_voltage_tick = 0;

    // Battery is sensed through a 1:3 resistor divider
    battery_voltage = adc1_getMeasurement(0) * 3 / 100;
	averageBatteryVoltage = battery_voltage;
	battery_voltage_tick=0;

	menuBatteryPushBackVoltage(battery_voltage);

    for(;;)
    {
		int tmp_battery_voltage = adc1_getMeasurement(0) * 3 / 100;
		if (battery_voltage != tmp_battery_voltage)
		{
			battery_voltage = tmp_battery_voltage;
			averageBatteryVoltage = (averageBatteryVoltage * (AVERAGE_BATTERY_VOLTAGE_SAMPLE_WINDOW-1) + battery_voltage) / AVERAGE_BATTERY_VOLTAGE_SAMPLE_WINDOW;
			batteryVoltageHasChanged = true;
		}

		menuBatteryPushBackVoltage(averageBatteryVoltage);
    }
}

void vApplicationTickHook() { }
void vApplicationStackOverflowHook() { }
void vApplicationIdleHook() { }
void vApplicationMallocFailedHook() { }
