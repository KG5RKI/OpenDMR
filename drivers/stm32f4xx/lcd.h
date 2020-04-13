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

#ifndef LCH_H
#define LCH_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

/**
 * Low level driver for Tytera MD380 display, which is has an HX8302-A controller.
 * Actually, no datasheet for the display controller exists on the internet,
 * however a fairly compatible chip, for which datasheet exists, is the HX8353-E.
 *
 *********************** HOW TO MANAGE FRAMEBUFFER *****************************
 *
 * This driver allocates the framebuffer as a block of memory addressed linearly
 * as an array of SCREEN_HEIGHT*SCREEN_WIDTH 16-bit variables.
 * With respect to it, screen is indexed in this way:
 *
 *   (0,0)
 *     +-------> x
 *     |
 *     |    o (X,Y)
 *     |
 *     v
 *     y
 *
 * then to set the value of the pixel having coordinates (X,Y), framebuffer has
 * to be indexed in this way buf[X + Y*SCREEN_WIDTH].
 *
 */

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128

/**
 * This function initialises the display, configures TIM8 for backlight control
 * and allocates framebuffer on the heap. After initialisation, backlight is
 * set to zero.
 * NOTE: framebuffer allocation is the first operation performed, if fails an
 * error message is printed on the virtual COM port and this function returns
 * prematurely, without configuring the display and the backlight timer. Thus, a
 * dark screen can be symptom of failed allocation.
 */
void lcd_init();

/**
 * When called, this function turns off backlight, shuts down TIM8 and
 * deallocates the framebuffer.
 */
void lcd_terminate();

/**
 * Set screen backlight to a given level.
 * @param level: backlight level, from 0 (backlight off) to 255 (backlight at
 * full brightness).
 */
void lcd_setBacklightLevel(uint8_t level);

/**
 * Copy a given section, between two given rows, of framebuffer content to the
 * display.
 * @param startRow: first row of the framebuffer section to be copied
 * @param endRow: last row of the framebuffer section to be copied
 */
void lcd_renderRows(uint8_t startRow, uint8_t endRow);

/**
 * Copy framebuffer content to the display internal buffer. To be called
 * whenever there is need to update the display.
 */
static inline void lcd_render()
{
    lcd_renderRows(0, SCREEN_HEIGHT);
}

/**
 * Check if framebuffer is being copied to the screen or not, in which case it
 * can be modified without problems.
 * @return false if rendering is not in progress.
 */
bool lcd_renderingInProgress();

/**
 * Get pointer to framebuffer. This buffer is addressed linearly and each
 * location is a pixel whose color coding is RGB565.
 * Changes to the framebuffer will not be reflected on the display until
 * lcd_render() or lcd_renderRows() are called.
 *
 * IMPORTANT NOTE: to accomodate the display driver chip's needs, this buffer
 * MUST be filled with values in big endian mode! A cleaner way to have the
 * correct endianness, is to use GCC's builtin function __builtin_bswap16().
 *
 * WARNING: no bound check is performed! Do not call free() on the pointer
 * returned, doing so will destroy the framebuffer!
 * @return pointer to framebuffer.
 */
uint16_t *lcd_getFrameBuffer();

#endif /* LCH_H */
