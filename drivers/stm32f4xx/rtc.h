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

#ifndef RTC_H
#define RTC_H

#include <stdint.h>
#include "stm32f4xx.h"

typedef struct
{
    uint8_t hour   : 5;    /* Hours (0-23)            */
    uint8_t minute : 6;    /* Minutes (0-59)          */
    uint8_t second : 6;    /* Seconds (0-59)          */
    uint8_t day    : 3;    /* Day of the week (1-7)   */
    uint8_t date   : 4;    /* Day of the month (1-31) */
    uint8_t month  : 4;    /* Month (1-12)            */
    uint8_t year   : 7;    /* Year (0-99)             */
    uint8_t        : 5;    /* Padding to 40 bits      */
}curTime_t;

/**
 * 
 */
void rtc_init();

/**
 * 
 */
void rtc_shutdown();

void rtc_setTime(curTime_t t);

void rtc_setHour(uint8_t hours, uint8_t minutes, uint8_t seconds);

void rtc_setDate(uint8_t date, uint8_t month, uint8_t year);

curTime_t rtc_getTime();

#endif /* RTC_H */
