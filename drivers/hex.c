#include "hex.h"
#include "dtekvio.h"
#define HEX_COUNT 6U /* DTEK-V has exactly 6 hex displays */
#define HEX_STRIDE 0x10U
#define HEX_NIBBLE_MASK 0x0FU
#define HEX_BLANK 0xFFU
#define HEX_REG(n)                                                             \
  (*(volatile uint32_t *)(MMIO_HEX_BASE + (uint32_t)(n) * HEX_STRIDE))

static const uint8_t seg7[16U] = {
    0xC0, 0xF9, 0xA4, 0xB0, /* 0, 1, 2, 3 */
    0x99, 0x92, 0x82, 0xF8, /* 4, 5, 6, 7 */
    0x80, 0x90, 0x88, 0x83, /* 8, 9, A, b */
    0xC6, 0xA1, 0x86, 0x8E  /* C, d, E, F */
};

void hex_write(uint8_t display, uint8_t val) {
  if (display < HEX_COUNT) {
    HEX_REG(display) = seg7[val & HEX_NIBBLE_MASK];
  }
}

void hex_clear(void) {
  for (uint8_t i = 0U; i < HEX_COUNT; i++) {
    HEX_REG(i) = HEX_BLANK;
  }
}

void hex_write_score(uint32_t score, uint32_t high_score) {
  /* 6 displays total: 0-2 = score, 3-5 = high score */
  uint32_t s = score % 1000U; /* clamp to 3 digits */
  uint32_t hs = high_score % 1000U;

  hex_write(0U, (uint8_t)(s % 10U));
  s /= 10U;
  hex_write(1U, (uint8_t)(s % 10U));
  s /= 10U;
  hex_write(2U, (uint8_t)(s % 10U));

  hex_write(3U, (uint8_t)(hs % 10U));
  hs /= 10U;
  hex_write(4U, (uint8_t)(hs % 10U));
  hs /= 10U;
  hex_write(5U, (uint8_t)(hs % 10U));
}