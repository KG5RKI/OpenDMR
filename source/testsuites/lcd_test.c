/***************************************************************************
 *   Copyright (C) 2020 by Silvano Seva IU2KWO                             *
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
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx.h"
#include "gpio.h"
#include "graphics.h"
#include "lcd.h"

void blink(void *arg)
{

    graphicsInit(COLOR_WHITE);
    clearRows(0, 8, COLOR_WHITE);
    lcd_setBacklightLevel(254);

    /* Horizontal red line */
    fillRect(0, 10, SCREEN_WIDTH, 20, 0xF800);
    /* Vertical blue line */
    fillRect(10, 0, 20, SCREEN_HEIGHT, 0x001F);

    /* Vertical green line */
    fillRect(80, 0, 20, SCREEN_HEIGHT, 0x07e0);

    char *buffer = "KEK";
    printCore(32, 32, buffer, FONT_SIZE_4, TEXT_ALIGN_LEFT, 0x0000);

    /* Example of split rendering, first render top half then bottom */
    //renderRows(4, 8);
    //renderRows(0, 4);

    /* We could have called directly this otherwise */
    render();

    while(1)
    {
        gpio_togglePin(GPIOE, 0);
        vTaskDelay(500);
    }
}

int main (void)
{
    gpio_setMode(GPIOD, 8, OUTPUT);
    gpio_setPin(GPIOD, 8);

    xTaskCreate(blink, "blink", 256, NULL, 0, NULL);
    vTaskStartScheduler();

    for(;;) ;
}

void vApplicationTickHook() { }
void vApplicationStackOverflowHook() { }
void vApplicationIdleHook() { }
void vApplicationMallocFailedHook() { }
