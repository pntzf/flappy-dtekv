#ifndef VGA_H
#define VGA_H

#include "types.h"

#define VGA_WIDTH 320
#define VGA_HEIGHT 240
#define VGA_SIZE (VGA_WIDTH * VGA_HEIGHT)

#define VGA_COLOR(r, g, b)                                                     \
  ((uint8_t)((((r) & 0x7U) << 5U) | (((g) & 0x7U) << 2U) | ((b) & 0x3U)))
#define VGA_BLACK 0x00U
#define VGA_WHITE 0xFFU

void vga_init(void);
void vga_fill(uint8_t color);
void vga_swap(void);
uint8_t *vga_back(void);

#endif /* VGA_H */
