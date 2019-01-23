/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2018-12-18     ZeroFree     first implementation
 */

#include "nimble/nimble_npl.h"
#include "nrfx.h"

static void (*radio_isr_addr)(void);
static void (*rng_isr_addr)(void);
static void (*rtc0_isr_addr)(void);

void RADIO_IRQHandler(void)
{
    radio_isr_addr();
}

void RNG_IRQHandler(void)
{
    rng_isr_addr();
}

void RTC0_IRQHandler(void)
{
    rtc0_isr_addr();
}

void ble_npl_hw_set_isr(int irqn, void (*addr)(void))
{
    switch (irqn)
    {
    case RADIO_IRQn:
        radio_isr_addr = addr;
        break;
    case RNG_IRQn:
        rng_isr_addr = addr;
        break;
    case RTC0_IRQn:
        rtc0_isr_addr = addr;
        break;
    }
}
