#include "button.h"
#include "game.h"
#include "hex.h"
#include "isr.h"
#include "leds.h"
#include "render.h"
#include "timer.h"
#include "types.h"
#include "vga.h"

#define CPU_HZ 30000000U
#define TARGET_FPS 60U
#define TICKS_PER_FRAME (CPU_HZ / TARGET_FPS)

static volatile uint8_t frame_ready = 0U;

static void
enable_interrupt(void) { /* enable machine timer + global interrupts */
  asm volatile("csrsi mie, 16");
  asm volatile("csrsi mstatus, 8");
}

static void timer_isr(void) {
  timer_clear_irq();
  frame_ready = 1U;
}

void handle_interrupt(uint32_t cause) {
  if (cause == TIMER_IRQ) {
    timer_isr();
  }
}

static void hw_init(void) {
  timer_init(TICKS_PER_FRAME);

  enable_interrupt();

  vga_init();
  leds_clear();
  button_init();
  hex_clear();
}

static void update_leds(void) {
  if (button_read() != FALSE) {
    leds_set_all(LEDS_MASK);
  } else {
    leds_clear();
  }
}

int main(void) {
  game_t g;

  hw_init();
  game_init(&g);
  frame_ready = 0U;

  for (;;) {
    while (frame_ready == 0U) {
      ;
    }
    frame_ready = 0U;

    if (button_was_pressed() != FALSE) {
      switch (g.state) {
      case STATE_MENU:
      case STATE_GAME_OVER:
        game_start(&g);
        break;

      case STATE_PLAYING:
        game_flap(&g);
        break;

      default:
        break;
      }
    }

    game_update(&g);

    render(&g);
    vga_swap();

    hex_write_score(g.score, g.high_score);
    update_leds();
  }

  return 0;
}
