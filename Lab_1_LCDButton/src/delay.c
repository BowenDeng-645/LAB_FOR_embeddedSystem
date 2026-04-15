#include <platform.h>
#include <stdint.h>
#include "delay.h"

void delay_ms(unsigned int ms) 
	{
		unsigned int max_step = 1000 * (UINT32_MAX / SystemCoreClock);
		unsigned int max_sleep_cycles = max_step * (SystemCoreClock / 1000);
		while (ms > max_step) {
			ms -= max_step;
			delay_cycles(max_sleep_cycles);
		}
		delay_cycles(ms * (SystemCoreClock / 1000));
	}

void delay_us(unsigned int us) 
	{
		unsigned int max_step = 1000000 * (UINT32_MAX / SystemCoreClock);
		unsigned int max_sleep_cycles = max_step * (SystemCoreClock / 1000000);
		while (us > max_step) {
			us -= max_step;
			delay_cycles(max_sleep_cycles);
		}
		delay_cycles(us * (SystemCoreClock / 1000000));
	}

__attribute__((naked)) void delay_cycles(unsigned int cycles)
{
    __asm volatile(
        "lsrs r0, r0, #2      \n"   // cycles /= 4
        "beq  2f              \n"   // if zero, done
        "1:                   \n"   // loop:
        "subs r0, r0, #1      \n"
#if (__CORTEX_M == 3) || (__CORTEX_M == 4)
        "nop                  \n"
#endif
        "bne  1b              \n"   // if not zero, loop
        "2:                   \n"   // done:
        "bx   lr              \n"
    );
}

// *******************************ARM University Program Copyright © ARM Ltd 2016*************************************
