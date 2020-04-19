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
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx.h"
#include "gpio.h"
#include "lcd.h"

/* Comment out to test full render */
#define TEST_SPLIT_RENDER

void blink(void *arg)
{
    lcd_init();
    lcd_setBacklightLevel(0xFF);

    uint16_t *fb = lcd_getFrameBuffer();
    size_t i = 0;

    #ifndef TEST_SPLIT_RENDER
    /* Upper half red and lower half green */
    for(; i < (SCREEN_HEIGHT*SCREEN_WIDTH)/2; i++)
    {
        fb[i] = __builtin_bswap16(0xF800); // red
    }

    for(; i < SCREEN_HEIGHT*SCREEN_WIDTH; i++)
    {
        fb[i] = __builtin_bswap16(0x07E0); // green
    }

    while(lcd_renderingInProgress()) ;
    lcd_render();
    vTaskDelay(500);

    /* Upper half green and lower half red */
    for(i = 0; i < (SCREEN_HEIGHT*SCREEN_WIDTH)/2; i++)
    {
        fb[i] = __builtin_bswap16(0x07E0); // green
    }

    for(; i < SCREEN_HEIGHT*SCREEN_WIDTH; i++)
    {
        fb[i] = __builtin_bswap16(0xF800); // red
    }

    while(lcd_renderingInProgress()) ;
    lcd_render();
    vTaskDelay(500);
    #endif

    /* All screen white */
    puts("Clearing screen\r");
    for(i = 0; i < SCREEN_HEIGHT*SCREEN_WIDTH; i++)
    {
        fb[i] = __builtin_bswap16(0xFFFF);
    }

    while(lcd_renderingInProgress()) ;
    lcd_render();

    #ifdef TEST_SPLIT_RENDER
    puts("Partial render 1\r");
    for(uint8_t y = 10; y < 20; y++)
    {
        fb[y*SCREEN_WIDTH] = __builtin_bswap16(0xF800);
    }

    while(lcd_renderingInProgress()) ;
    lcd_renderRows(10, 20);

    puts("Partial render 2\r");
    for(uint8_t y = 40; y < 60; y++)
    {
        fb[y*SCREEN_WIDTH] = __builtin_bswap16(0x001F);
    }

    while(lcd_renderingInProgress()) ;
    lcd_renderRows(40, 60);
    #endif

    puts("Render test end\r");

    while(1)
    {
        vTaskDelay(500);
    }
}

int main (void)
{
    gpio_setMode(GPIOE, 0, OUTPUT);

    xTaskCreate(blink, "blink", 256, NULL, 0, NULL);
    vTaskStartScheduler();

    for(;;) ;
}

void vApplicationTickHook() { }
void vApplicationStackOverflowHook() { }
void vApplicationIdleHook() { }
void vApplicationMallocFailedHook() { }
