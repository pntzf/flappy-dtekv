#include "timer.h"
#include "dtekvio.h"

typedef struct {
  volatile uint32_t status;
  volatile uint32_t control;
  volatile uint32_t periodl;
  volatile uint32_t periodh;
} timer_t;

#define CTRL_ITO (1U << 0U)  /* interrupt on timeout */
#define CTRL_CONT (1U << 1U) /* continuous mode */
#define CTRL_START (1U << 2U)
#define STATUS_TO (1U << 0U) /* timeout flag */

static timer_t *const REGS = (timer_t *)MMIO_TIMER_BASE;

void timer_init(uint32_t period) {
  REGS->control = 0U;
  REGS->periodl = (uint16_t)((period - 1U) & 0xFFFFU);
  REGS->periodh = (uint16_t)((period - 1U) >> 16U);
  REGS->control = CTRL_ITO | CTRL_CONT | CTRL_START;
}

void timer_clear_irq(void) { REGS->status = STATUS_TO; }
