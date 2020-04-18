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

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "HX83XX_charset.h"
#include "graphics.h"
#include "lcd.h"

const int FONT_SIZE_3_HEIGHT = 8;

#define INLINE __attribute__((always_inline)) inline
#define LE2BE(x) __builtin_bswap16(x)
// number representing the maximum angle (e.g. if 100, then if you pass in start=0 and end=50, you get a half circle)
// this can be changed with setArcParams function at runtime
#define DEFAULT_ARC_ANGLE_MAX 360
// rotational offset in degrees defining position of value 0 (-90 will put it at the top of circle)
// this can be changed with setAngleOffset function at runtime
#define DEFAULT_ANGLE_OFFSET -90
static float _arcAngleMax = DEFAULT_ARC_ANGLE_MAX;
static float _angleOffset = DEFAULT_ANGLE_OFFSET;
#define DEG_TO_RAD  0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define swap(x, y) do { typeof(x) t = x; x = y; y = t; } while(0)

#define FB_SIZE SCREEN_WIDTH * SCREEN_HEIGHT * 2
#define COLOR_DEPTH 16
#define N_ROWS 8
#define ROW_HEIGHT 16

uint16_t *screenBuf;

/*
 * These two colors will be used as background,foreground and font color
 * unless otherwise specified
 */
uint16_t bgColor;
uint16_t fgColor;
uint16_t fontColor;

INLINE void graphicsInit(uint16_t bg, uint16_t fg, uint16_t font)
{
    bgColor = bg;
    fgColor = fg;
    fontColor = font;
    lcd_init();
    screenBuf = lcd_getFrameBuffer();
    clearBuf();
    //render();
}

INLINE void clearBuf(void)
{
    for(int i = 0; i < FB_SIZE / 2; i++)
        screenBuf[i] = bgColor;
}

INLINE void clearRows(int16_t startRow, int16_t endRow, uint16_t backgroundColor)
{
	// Boundaries
	if (((startRow < 0) || (endRow < 0)) ||
        ((startRow > N_ROWS) || (endRow > N_ROWS)) || (startRow == endRow))
		return;

	if (endRow < startRow)
	{
		swap(startRow, endRow);
	}

    for(uint16_t i = startRow * ROW_HEIGHT * SCREEN_WIDTH;
        i <= endRow * ROW_HEIGHT * SCREEN_WIDTH; i++)
        screenBuf[i] = LE2BE(backgroundColor);
}

INLINE void render(void)
{
    /* Wait for previous rendering operations to terminate */
    while (lcd_renderingInProgress()) ;
	renderRows(0,N_ROWS);
}

INLINE void renderRows(int16_t startRow, int16_t endRow)
{
    /* Wait for previous rendering operations to terminate */
    while (lcd_renderingInProgress()) ;
    lcd_renderRows(startRow * ROW_HEIGHT, endRow * ROW_HEIGHT);
}

INLINE void printCentered(uint8_t y, const  char *text, font_t fontSize)
{
	printCore(0, y, text, fontSize, TEXT_ALIGN_CENTER, fontColor);
}

INLINE void printAt(uint8_t x, uint8_t y,const  char *text, font_t fontSize)
{
	printCore(x, y, text, fontSize, TEXT_ALIGN_LEFT, fontColor);
}

INLINE int printCore(int16_t x, int16_t y, const char *szMsg, font_t fontSize, textAlign_t alignment, uint16_t color)
{
	int16_t i, sLen;
	uint8_t *currentCharData;
	int16_t charWidthPixels;
	int16_t charHeightPixels;
	int16_t bytesPerChar;
	int16_t startCode;
	int16_t endCode;
	uint8_t *currentFont;
	uint16_t *writePos;
	uint8_t *readPos;

    sLen = strlen(szMsg);

    switch(fontSize)
    {
    	case FONT_SIZE_1:
    		currentFont = (uint8_t *) font_6x8;
    		break;
    	case FONT_SIZE_1_BOLD:
			currentFont = (uint8_t *) font_6x8_bold;
    		break;
    	case FONT_SIZE_2:
    		currentFont = (uint8_t *) font_8x8;
    		break;
    	case FONT_SIZE_3:
    		currentFont = (uint8_t *) font_8x16;
			break;
    	case FONT_SIZE_4:
    		currentFont = (uint8_t *) font_16x32;
			break;
    	default:
    		return -2;// Invalid font selected
    		break;
    }

    startCode   		= currentFont[2];  // get first defined character
    endCode 	  		= currentFont[3];  // get last defined character
    charWidthPixels   	= currentFont[4];  // width in pixel of one char
    charHeightPixels  	= currentFont[5];  // page count per char
    bytesPerChar 		= currentFont[7];  // bytes per char

    // Compute amount of letters that fit till the end of the screen
    if ((charWidthPixels*sLen) + x > SCREEN_WIDTH)
	{
    	sLen = (SCREEN_WIDTH-x)/charWidthPixels;
	}

	if (sLen < 0)
	{
		return -1;
	}

	switch(alignment)
	{
		case TEXT_ALIGN_LEFT:
			// left aligned, do nothing.
			break;
		case TEXT_ALIGN_CENTER:
			x = (SCREEN_WIDTH - (charWidthPixels * sLen))/2;
			break;
		case TEXT_ALIGN_RIGHT:
			x = SCREEN_WIDTH - (charWidthPixels * sLen);
			break;
	}

	for (i=0; i<sLen; i++)
	{
		uint32_t charOffset = (szMsg[i] - startCode);

		// End boundary checking.
		if (charOffset > endCode)
		{
			charOffset = ('?' - startCode); // Substitute unsupported ASCII code by a question mark
		}

		currentCharData = (uint8_t *)&currentFont[8 + (charOffset * bytesPerChar)];

        // We print the character from up-left to bottom right
        for(int16_t vscan=0; vscan < charHeightPixels; vscan++) {
            for(int16_t hscan=0; hscan < charWidthPixels; hscan++) {
                int16_t charChunk = vscan / 8;
                int16_t bitIndex = (hscan + charChunk * charWidthPixels) * 8 +
                                   vscan % 8;
                int16_t byte = bitIndex >> 3;
                int16_t bitMask = 1 << (bitIndex & 7);
                if (currentCharData[byte] & bitMask)
                    screenBuf[(y + vscan) * SCREEN_WIDTH +
                               x + hscan + i * charWidthPixels] = LE2BE(color);
            }
        }
	}
	return 0;
}

INLINE int16_t setPixel(int16_t x, int16_t y, uint16_t color)
{
    if (x < 0 || x > SCREEN_WIDTH || y < 0 || y > SCREEN_HEIGHT)
		return -1; // off the screen

    // Framebuffer is Big Endian, fix endianness before writing
    screenBuf[x+y*SCREEN_WIDTH] = LE2BE(color);
	return 0;
}

INLINE void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);

	if (steep)
	{
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1)
	{
		swap(x0, x1);
		swap(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1)
		ystep = 1;
	else
		ystep = -1;

	for (; x0<=x1; x0++)
	{
		if (steep)
			setPixel(y0, x0, color);
		else
			setPixel(x0, y0, color);

		err -= dy;
		if (err < 0)
		{
			y0 += ystep;
			err += dx;
		}
	}
}

INLINE void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	fillRect(x, y, 1, h, color);
}

INLINE void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	fillRect(x, y, w, 1, color);
}

INLINE void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	setPixel(x0    , y0 + r, color);
	setPixel(x0    , y0 - r, color);
	setPixel(x0 + r, y0    , color);
	setPixel(x0 - r, y0    , color);

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}

		x++;
		ddF_x += 2;
		f += ddF_x;

		setPixel(x0 + x, y0 + y, color);
		setPixel(x0 - x, y0 + y, color);
		setPixel(x0 + x, y0 - y, color);
		setPixel(x0 - x, y0 - y, color);
		setPixel(x0 + y, y0 + x, color);
		setPixel(x0 - y, y0 + x, color);
		setPixel(x0 + y, y0 - x, color);
		setPixel(x0 - y, y0 - x, color);
	}
}

void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}

		x++;
		ddF_x += 2;
		f     += ddF_x;

		if (cornername & 0x4)
		{
			setPixel(x0 + x, y0 + y, color);
			setPixel(x0 + y, y0 + x, color);
		}

		if (cornername & 0x2)
		{
			setPixel(x0 + x, y0 - y, color);
			setPixel(x0 + y, y0 - x, color);
		}

		if (cornername & 0x8)
		{
			setPixel(x0 - y, y0 + x, color);
			setPixel(x0 - x, y0 + y, color);
		}

		if (cornername & 0x1)
		{
			setPixel(x0 - y, y0 - x, color);
			setPixel(x0 - x, y0 - y, color);
		}
	}
}

void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, bool color)
{
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}

		x++;
		ddF_x += 2;
		f     += ddF_x;

		if (cornername & 0x1)
		{
			drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
			drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
		}

		if (cornername & 0x2)
		{
			drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
			drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
		}
	}
}

INLINE void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	drawFastVLine(x0, y0 - r, 2 * r + 1, color);
	fillCircleHelper(x0, y0, r, 3, 0, color);
}

INLINE void drawEllipse(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  int16_t a = abs(x1 - x0), b = abs(y1 - y0), b1 = b & 1; /* values of diameter */
  long dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a; /* error increment */
  long err = dx + dy + b1 * a * a, e2; /* error of 1.step */

  if (x0 > x1) { x0 = x1; x1 += a; } /* if called with swapped points */
  if (y0 > y1) y0 = y1; /* .. exchange them */
  y0 += (b + 1) / 2; /* starting pixel */
  y1 = y0 - b1;
  a *= 8 * a;
  b1 = 8 * b * b;

  do {
	  setPixel(x1, y0, color); /*   I. Quadrant */
	  setPixel(x0, y0, color); /*  II. Quadrant */
	  setPixel(x0, y1, color); /* III. Quadrant */
	  setPixel(x1, y1, color); /*  IV. Quadrant */
    e2 = 2 * err;
    if (e2 >= dx) { x0++; x1--; err += dx += b1; } /* x step */
    if (e2 <= dy) { y0++; y1--; err += dy += a; }  /* y step */
  } while (x0 <= x1);

  while (y0 - y1 < b) /* too early stop of flat ellipses a=1 */
  {
	  setPixel(x0 - 1, ++y0, color); /* -> complete tip of ellipse */
	  setPixel(x0 - 1, --y1, color);
  }
}

INLINE void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	drawLine(x0, y0, x1, y1, color);
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x0, y0, color);
}

INLINE void fillTriangle ( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	int16_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1)
	{
		swap(y0, y1); swap(x0, x1);
	}
	if (y1 > y2)
	{
		swap(y2, y1); swap(x2, x1);
	}
	if (y0 > y1)
	{
		swap(y0, y1); swap(x0, x1);
	}

	if(y0 == y2) // Handle awkward all-on-same-line case as its own thing
	{
		a = b = x0;
		if(x1 < a)      a = x1;
		else if(x1 > b) b = x1;

		if(x2 < a)      a = x2;
		else if(x2 > b) b = x2;

		drawFastHLine(a, y0, b - a + 1, color);
		return;
	}

	int16_t dx01 = x1 - x0,
			dy01 = y1 - y0,
			dx02 = x2 - x0,
			dy02 = y2 - y0,
			dx12 = x2 - x1,
			dy12 = y2 - y1,
			sa   = 0,
			sb   = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if(y1 == y2) last = y1;   // Include y1 scanline
	else         last = y1-1; // Skip it

	for(y=y0; y<=last; y++) {
		a   = x0 + sa / dy01;
		b   = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
		a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if(a > b)
			swap(a,b);

		drawFastHLine(a, y, b - a + 1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);

	for(; y<=y2; y++)
	{
		a   = x1 + sa / dy12;
		b   = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
		a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if(a > b)
			swap(a,b);

		drawFastHLine(a, y, b - a + 1, color);
	}
}

/*
 * ***** Arc related functions *****
 */
static float cosDegrees(float angle)
{
	return cos(angle * DEG_TO_RAD);
}

static float sinDegrees(float angle)
{
	return sin(angle * DEG_TO_RAD);
}

void fillArcOffsetted(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, float start, float end, bool color)
{
	int16_t xmin = 65535, xmax = -32767, ymin = 32767, ymax = -32767;
	float cosStart, sinStart, cosEnd, sinEnd;
	float r, t;
	float startAngle, endAngle;

	startAngle = (start / _arcAngleMax) * 360;	// 252
	endAngle = (end / _arcAngleMax) * 360;		// 807

	while (startAngle < 0) startAngle += 360;
	while (endAngle < 0) endAngle += 360;
	while (startAngle > 360) startAngle -= 360;
	while (endAngle > 360) endAngle -= 360;

	if (startAngle > endAngle)
	{
		fillArcOffsetted(cx, cy, radius, thickness, ((startAngle) / (float)360) * _arcAngleMax, _arcAngleMax, color);
		fillArcOffsetted(cx, cy, radius, thickness, 0, ((endAngle) / (float)360) * _arcAngleMax, color);
	}
	else
	{
		// Calculate bounding box for the arc to be drawn
		cosStart = cosDegrees(startAngle);
		sinStart = sinDegrees(startAngle);
		cosEnd = cosDegrees(endAngle);
		sinEnd = sinDegrees(endAngle);

		r = radius;
		// Point 1: radius & startAngle
		t = r * cosStart;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinStart;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;

		// Point 2: radius & endAngle
		t = r * cosEnd;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinEnd;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;

		r = radius - thickness;
		// Point 3: radius-thickness & startAngle
		t = r * cosStart;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinStart;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;

		// Point 4: radius-thickness & endAngle
		t = r * cosEnd;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinEnd;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;

		// Corrections if arc crosses X or Y axis
		if ((startAngle < 90) && (endAngle > 90))
		{
			ymax = radius;
		}

		if ((startAngle < 180) && (endAngle > 180))
		{
			xmin = -radius;
		}

		if ((startAngle < 270) && (endAngle > 270))
		{
			ymin = -radius;
		}

		// Slopes for the two sides of the arc
		float sslope = (float)cosStart / (float)sinStart;
		float eslope = (float)cosEnd / (float)sinEnd;

		if (endAngle == 360) eslope = -1000000;

		int ir2 = (radius - thickness) * (radius - thickness);
		int or2 = radius * radius;

		for (int x = xmin; x <= xmax; x++)
		{
			bool y1StartFound = false, y2StartFound = false;
			bool y1EndFound = false, y2EndSearching = false;
			int y1s = 0, y1e = 0, y2s = 0;
			for (int y = ymin; y <= ymax; y++)
			{
				int x2 = x * x;
				int y2 = y * y;

				if (
					(x2 + y2 < or2 && x2 + y2 >= ir2) && (
					(y > 0 && startAngle < 180 && x <= y * sslope) ||
					(y < 0 && startAngle > 180 && x >= y * sslope) ||
					(y < 0 && startAngle <= 180) ||
					(y == 0 && startAngle <= 180 && x < 0) ||
					(y == 0 && startAngle == 0 && x > 0)
					) && (
					(y > 0 && endAngle < 180 && x >= y * eslope) ||
					(y < 0 && endAngle > 180 && x <= y * eslope) ||
					(y > 0 && endAngle >= 180) ||
					(y == 0 && endAngle >= 180 && x < 0) ||
					(y == 0 && startAngle == 0 && x > 0)))
				{
					if (!y1StartFound)	//start of the higher line found
					{
						y1StartFound = true;
						y1s = y;
					}
					else if (y1EndFound && !y2StartFound) //start of the lower line found
					{
						y2StartFound = true;
						//drawPixel_cont(cx+x, cy+y, ILI9341_BLUE);
						y2s = y;
						y += y1e - y1s - 1;	// calculate the most probable end of the lower line (in most cases the length of lower line is equal to length of upper line), in the next loop we will validate if the end of line is really there
						if (y > ymax - 1) // the most probable end of line 2 is beyond ymax so line 2 must be shorter, thus continue with pixel by pixel search
						{
							y = y2s;	// reset y and continue with pixel by pixel search
							y2EndSearching = true;
						}
					}
					else if (y2StartFound && !y2EndSearching)
					{
						// we validated that the probable end of the lower line has a pixel, continue with pixel by pixel search, in most cases next loop with confirm the end of lower line as it will not find a valid pixel
						y2EndSearching = true;
					}
				}
				else
				{
					if (y1StartFound && !y1EndFound) //higher line end found
					{
						y1EndFound = true;
						y1e = y - 1;
						drawFastVLine(cx + x, cy + y1s, y - y1s, color);
						if (y < 0)
						{
							y = abs(y); // skip the empty middle
						}
						else
							break;
					}
					else if (y2StartFound)
					{
						if (y2EndSearching)
						{
							// we found the end of the lower line after pixel by pixel search
							drawFastVLine(cx + x, cy + y2s, y - y2s, color);
							y2EndSearching = false;
							break;
						}
						else
						{
							// the expected end of the lower line is not there so the lower line must be shorter
							y = y2s;	// put the y back to the lower line start and go pixel by pixel to find the end
							y2EndSearching = true;
						}
					}
				}
			}
			if (y1StartFound && !y1EndFound)
			{
				y1e = ymax;
				drawFastVLine(cx + x, cy + y1s, y1e - y1s + 1, color);
			}
			else if (y2StartFound && y2EndSearching)	// we found start of lower line but we are still searching for the end
			{										// which we haven't found in the loop so the last pixel in a column must be the end
				drawFastVLine(cx + x, cy + y2s, ymax - y2s + 1, color);
			}
		}
	}
}

INLINE void fillArc(uint16_t x, uint16_t y, uint16_t radius, uint16_t thickness, float start, float end, uint16_t color)
{
	if (start == 0 && end == _arcAngleMax)
		fillArcOffsetted(x, y, radius, thickness, 0, _arcAngleMax, color);
	else
		fillArcOffsetted(x, y, radius, thickness, start + (_angleOffset / (float)360)*_arcAngleMax, end + (_angleOffset / (float)360)*_arcAngleMax, color);
}

INLINE void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
	// smarter version
	drawFastHLine(x + r    , y        , w - 2 * r, color); // Top
	drawFastHLine(x + r    , y + h - 1, w - 2 * r, color); // Bottom
	drawFastVLine(x        , y + r    , h - 2 * r, color); // Left
	drawFastVLine(x + w - 1, y + r    , h - 2 * r, color); // Right
	// draw four corners
	drawCircleHelper(x + r        , y + r        , r, 1, color);
	drawCircleHelper(x + w - r - 1, y + r        , r, 2, color);
	drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
	drawCircleHelper(x + r        , y + h - r - 1, r, 8, color);
}

INLINE void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
	fillRect(x + r, y, w - 2 * r, h, color);

	// draw four corners
	fillCircleHelper(x+w-r-1, y + r, r, 1, h - 2 * r - 1, bgColor);
	fillCircleHelper(x+r    , y + r, r, 2, h - 2 * r - 1, bgColor);
}

INLINE void drawRoundRectWithDropShadow(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
	fillRoundRect(x + 2, y, w, h, r, color); // Shadow
	fillRoundRect(x, y - 2, w, h, r, bgColor); // Empty box
	drawRoundRect(x, y - 2, w, h, r, color); // Outline
}

INLINE void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	drawFastHLine(x        , y        , w, color);
	drawFastHLine(x        , y + h - 1, w, color);
	drawFastVLine(x        , y        , h, color);
	drawFastVLine(x + w - 1, y        , h, color);
}

INLINE void fillRect(int16_t x, int16_t y, int16_t width, int16_t height, uint16_t color)
{
    for(uint16_t vscan = y; vscan < y+height; vscan++) {
        for(uint16_t hscan = x; hscan < x+width; hscan++) {
            screenBuf[vscan * SCREEN_WIDTH + hscan] = LE2BE(color);
        }
    }
}

INLINE void drawRectWithDropShadow(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	fillRect(x + 2, y, w, h, !color); // Shadow
	fillRect(x, y - 2, w, h, color); // Empty box
	drawRect(x, y - 2, w, h, color); // Outline
}

/*
 * Draw a 1-bit image at the specified (x,y) position.
*/
// TODO: this is probably broken
INLINE void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for(int16_t j = 0; j < h; j++, y++)
    {
        for(int16_t i = 0; i < w; i++)
        {
            if(i & 7)
            	byte <<= 1;
            else
            	byte = *(bitmap + (j * byteWidth + i / 8));

            if(byte & 0x80)
            	setPixel(x + i, y, color);
        }
    }
}

/*
 * Draw XBitMap Files (*.xbm), e.g. exported from GIMP.
*/
// TODO: this is probably broken
INLINE void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for(int16_t j = 0; j < h; j++, y++)
    {
        for(int16_t i = 0; i < w; i++)
        {
            if(i & 7)
            	byte >>= 1;
            else
            	byte = *(bitmap + (j * byteWidth + i / 8));
            // Nearly identical to drawBitmap(), only the bit order
            // is reversed here (left-to-right = LSB to MSB):
            if(byte & 0x01)
            	setPixel(x + i, y, color);
        }
    }
}

//INLINE void setContrast(uint8_t contrast);
//INLINE void setInverseVideo(bool isInverted);
//
//INLINE void drawChoice(choice_t choice, bool clearRegion);

INLINE void *getDisplayBuffer(void)
{
    return screenBuf;
}
