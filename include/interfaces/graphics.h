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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Generic display interface for abstracting the low level details of
 * radios of different shapes and sizes.
 *
 */

// Standard colors
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF

// Some radios have different height for the font size 3
extern const int FONT_SIZE_3_HEIGHT;

typedef enum
{
	FONT_SIZE_1 = 0,
	FONT_SIZE_1_BOLD,
	FONT_SIZE_2,
	FONT_SIZE_3,
	FONT_SIZE_4
} font_t;

typedef enum
{
	TEXT_ALIGN_LEFT = 0,
	TEXT_ALIGN_CENTER,
	TEXT_ALIGN_RIGHT
} textAlign_t;

typedef enum
{
	CHOICE_OK = 0,
	CHOICE_YESNO,
	CHOICE_DISMISS,
	CHOICES_NUM
} choice_t;

void graphicsInit(uint16_t bg, uint16_t fg, uint16_t font);
void clearBuf(void);
void clearRows(int16_t startRow, int16_t endRow, uint16_t backgroundColor);
void render(void);
void renderRows(int16_t startRow, int16_t endRow);
void printCentered(uint8_t y, const  char *text, font_t fontSize);
void printAt(uint8_t x, uint8_t y,const  char *text, font_t fontSize);
int printCore(int16_t x, int16_t y,const char *szMsg, font_t fontSize, textAlign_t alignment, uint16_t color);

int16_t setPixel(int16_t x, int16_t y, uint16_t color);

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

void drawEllipse(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void fillTriangle ( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

void fillArc(uint16_t x, uint16_t y, uint16_t radius, uint16_t thickness, float start, float end, uint16_t color);

void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void drawRoundRectWithDropShadow(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void fillRect(int16_t x, int16_t y, int16_t width, int16_t height, uint16_t color);
void drawRectWithDropShadow(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);

void setContrast(uint8_t contrast);
void setInverseVideo(bool isInverted);

void drawChoice(choice_t choice, bool clearRegion);

void * getDisplayBuffer(void);

#endif /* GRAPHICS_H */
