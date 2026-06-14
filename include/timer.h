#ifndef TIMER_H
#define TIMER_H

#include "types.h"

#define TIMER_IRQ 16

void timer_init(uint32_t period);
void timer_clear_irq(void);

#endif /* TIMER_H */
