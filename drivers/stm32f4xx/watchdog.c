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

#include "watchdog.h"

void wdt_configure(enum WdtPrescaler psc, uint16_t reloadValue)
{
    /*
     * Counter reload value and prescaler value can be updated only if RVU and
     * PVU bits in status register are cleared
     */
    while(IWDG->SR & (IWDG_SR_RVU | IWDG_SR_PVU)) ;

    IWDG->KR = 0x5555;                /* Unlock write access */
    IWDG->PR = psc;                     /* Set prescaler value */
    IWDG->RLR = (reloadValue & 0xFFF);  /* Set reload value    */
}

void wdt_start()
{
    IWDG->KR = 0xCCCC;
}

void wdt_reset()
{
    IWDG->KR = 0xAAAA;
}
