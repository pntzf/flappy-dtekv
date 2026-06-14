#include "sprite.h"
#include "vga.h"

#define TRANSPARENT 0xE3

static void blit(int32_t x, int32_t y, int32_t w, int32_t h,
                 const uint8_t *base, int32_t stride) {

  /* Calculate visible region and clip to screen boundaries */
  int32_t x0 = (x < 0) ? -x : 0;
  int32_t y0 = (y < 0) ? -y : 0;
  int32_t x1 = ((x + w) > VGA_WIDTH) ? VGA_WIDTH - x : w;
  int32_t y1 = ((y + h) > VGA_HEIGHT) ? VGA_HEIGHT - y : h;

  int32_t draw_width = x1 - x0;

  if (draw_width <= 0 || y1 <= y0) {
    return;
  }

  uint8_t *const vga_buf = vga_back();
  const uint8_t *src_row = base + (y0 * stride) + x0;
  uint8_t *dst_row = vga_buf + ((y + y0) * VGA_WIDTH) + (x + x0);

  for (int32_t row = y0; row < y1; row++) {
    const uint8_t *src_ptr = src_row;
    uint8_t *dst_ptr = dst_row;
    const uint8_t *end_ptr = src_ptr + draw_width;

    for (; src_ptr < end_ptr; src_ptr++, dst_ptr++) {
      uint8_t px = *src_ptr;
      if (px != TRANSPARENT) {
        *dst_ptr = px;
      }
    }

    src_row += stride;
    dst_row += VGA_WIDTH;
  }
}

void sprite_draw_clipped(int32_t x, int32_t y, const sprite_t *s,
                         int32_t clip_h) {
  blit(x, y, s->width, clip_h, s->data, s->width);
}

void sprite_draw(int32_t x, int32_t y, const sprite_t *s) {
  sprite_draw_clipped(x, y, s, s->height);
}

void sprite_draw_flipped(int32_t x, int32_t y, const sprite_t *s) {
  /* Start at the bottom row and use a negative stride to draw upside down */
  const uint8_t *bottom_row = s->data + ((s->height - 1) * s->width);
  blit(x, y, s->width, s->height, bottom_row, -s->width);
}
