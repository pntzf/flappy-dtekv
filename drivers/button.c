#include "button.h"
#include "dtekvio.h"

#define BUTTON_BIT 0x1U

typedef struct {
  volatile uint32_t data;
  volatile uint32_t _reserved;
  volatile uint32_t interrupt_mask;
  volatile uint32_t edge_capture;
} button_regs_t;

#define REGS ((button_regs_t *)(MMIO_BUTTON_BASE))

static bool_t last_state = FALSE;

void button_init(void) {
  REGS->interrupt_mask = 0U;
  REGS->edge_capture = BUTTON_BIT;
  last_state = FALSE;
}

bool_t button_read(void) { return (bool_t)((REGS->data & BUTTON_BIT) != 0U); }

bool_t button_was_pressed(void) {
  bool_t current = button_read();
  bool_t edge = (bool_t)((current != FALSE) && (last_state == FALSE));
  last_state = current;
  return edge;
}