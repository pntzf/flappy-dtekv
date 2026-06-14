#ifndef SPRITE_H
#define SPRITE_H

#include "types.h"

typedef struct {
  const uint8_t *data;
  int32_t width;
  int32_t height;
} sprite_t;

void sprite_draw(int32_t x, int32_t y, const sprite_t *s);
void sprite_draw_clipped(int32_t x, int32_t y, const sprite_t *s, int32_t h);
void sprite_draw_flipped(int32_t x, int32_t y, const sprite_t *s);

#endif /* SPRITE_H */
