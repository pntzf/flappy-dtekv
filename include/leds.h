#ifndef LEDS_H
#define LEDS_H

#include "types.h"

#define LED_COUNT (10U)
#define LEDS_MASK ((1U << LED_COUNT) - 1U)

void leds_clear(void);
void leds_set_all(uint32_t mask);

#endif /* LEDS_H */