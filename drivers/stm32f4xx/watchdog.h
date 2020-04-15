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

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <stdint.h>
#include "stm32f4xx.h"

/**
 * Driver for STM32 independent watchdog timer.
 * This timer is clocked with a dedicated RC oscillator whose frequency can
 * vary from 30kHz to 60kHz.
 *
 * The minimum and maximum timeout periods as function of prescaler division
 * factor and reload value, for a 32kHz clock, are given at page 629 of
 * STM32F4xx reference manual (RM0090).
 */

enum WdtPrescaler
{
    WDT_DIV4   = 0,
    WDT_DIV8   = 1,
    WDT_DIV16  = 2,
    WDT_DIV32  = 3,
    WDT_DIV64  = 4,
    WDT_DIV128 = 5,
    WDT_DIV256 = 6
};

/**
 * Configure clock prescaler and reload value for the watchdog timer.
 * @param psc: prescaler division factor
 * @param reloadValue: value to be loaded in prescaler counter when wdt_reset()
 * is called. Accepted range: 0h - FFFh
 */
void wdt_configure(enum WdtPrescaler psc, uint16_t reloadValue);

/**
 * Start the watchdog timer. Once started, this timer can be stopped only with
 * a power-on reset.
 */
void wdt_start();

/**
 * Reset watchdog counter, reloading the initial value.
 */
void wdt_reset();

#endif /* WATCHDOG_H */
