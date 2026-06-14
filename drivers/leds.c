#include "leds.h"
#include "dtekvio.h"

typedef struct {
  volatile uint32_t data;
} leds_regs_t;

#define REGS ((leds_regs_t *)(MMIO_LEDS_BASE))

void leds_clear(void) { REGS->data = 0U; }

void leds_set_all(uint32_t mask) { REGS->data = mask & LEDS_MASK; }
