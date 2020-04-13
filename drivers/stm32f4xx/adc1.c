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

#include "adc1.h"
#include "gpio.h"

static uint16_t measurements[4];

void adc1_init()
{
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    /* Configure GPIOs to analog input mode */
    gpio_setMode(GPIOA, 0, INPUT_ANALOG);
    gpio_setMode(GPIOA, 1, INPUT_ANALOG);
    gpio_setMode(GPIOA, 3, INPUT_ANALOG);
    gpio_setMode(GPIOB, 8, INPUT_ANALOG);

    /*
     * ADC clock is APB2 frequency divided by 8, giving 10.5MHz.
     * We set the sample time of each channel to 480 ADC cycles and we have to
     * scan four channels: this gives that the effective sampling frequency of
     * each channel is ~5.47kHz.
     */
    ADC->CCR |= ADC_CCR_ADCPRE;
    ADC1->SMPR2 = ADC_SMPR2_SMP0
                | ADC_SMPR2_SMP1
                | ADC_SMPR2_SMP3
                | ADC_SMPR2_SMP8;

    /*
     * No overrun interrupt, 12-bit resolution, no analog watchdog, no
     * discontinuous mode, enable scan mode, no end of conversion interrupts.
     */
    ADC1->CR1 |= ADC_CR1_SCAN;
    ADC2->CR2 |= ADC_CR2_DMA | ADC_CR2_ADON;

    /* Scan sequence config. */
    ADC1->SQR1 = 3 << 20;    /* Four channels to be converted          */
    ADC1->SQR3 |= (1 << 0)   /* CH1, battery voltage on PA1            */
               |  (8 << 5)   /* CH8, RSSI value on PB0                 */
               |  (3 << 10)  /* CH3, vox level on PA3                  */
               |  (0 << 15); /* CH0, volume potentiometer level on PA0 */

    /* DMA2 Stream 0 configuration:
     * - channel 0: ADC1
     * - low priority
     * - half-word transfer, both memory and peripheral
     * - increment memory
     * - circular mode
     * - peripheral-to-memory transfer
     * - no interrupts
     */
    DMA2_Stream0->PAR = ((uint32_t) &(ADC1->DR));
    DMA2_Stream0->M0AR = ((uint32_t) &measurements);
    DMA2_Stream0->CR = DMA_SxCR_MSIZE_0
                     | DMA_SxCR_PSIZE_0
                     | DMA_SxCR_MINC
                     | DMA_SxCR_CIRC
                     | DMA_SxCR_PFCTRL
                     | DMA_SxCR_EN;
}

void adc1_shutdown()
{
    RCC->APB2ENR &= ~RCC_APB2ENR_ADC1EN;
}

void adc1_start()
{
    ADC1->CR2 |= ADC_CR2_SWSTART;
}


uint16_t adc1_getMeasurement(uint8_t ch)
{
    if(ch > 3) return 0;
    return measurements[ch];
}
