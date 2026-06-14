#include "vga.h"
#include "dtekvio.h"

#define VGA_CTRL ((volatile uint32_t *)MMIO_VGA_CTRL)

#define VGA_REG_BUFFER 0
#define VGA_REG_BACKBUF 1
#define VGA_REG_STATUS 3
#define VGA_STATUS_SWAP 0x1U

/*
 * The dedicated video memory region holds exactly two frames:
 * 0x8000000–0x8025800 = 153600 bytes = 2 * 76800 (320*240).
 * We page-flip between them: the DMA scans one out while we render
 * into the other. No framebuffer copy is ever needed.
 */
#define VGA_FB0 ((uint8_t *)MMIO_VGA_FB_BASE)
#define VGA_FB1 ((uint8_t *)(MMIO_VGA_FB_BASE + VGA_SIZE))

static uint8_t *draw_buf;

static void wait_swap(void) {
  while ((VGA_CTRL[VGA_REG_STATUS] & VGA_STATUS_SWAP) != 0U) {
    ;
  }
}

void vga_init(void) {
  /* Video RAM is not zeroed by boot.S, so clear both framebuffers to black. */
  uint8_t *p = VGA_FB0;
  for (uint32_t i = 0U; i < (VGA_SIZE * 2U); i++) {
    p[i] = 0U;
  }

  VGA_CTRL[VGA_REG_BACKBUF] = (uint32_t)VGA_FB0;
  VGA_CTRL[VGA_REG_BUFFER] = 0U; /* any write = swap request */
  wait_swap();
  /* FB0 is now displayed; render into FB1 */
  draw_buf = VGA_FB1;
  VGA_CTRL[VGA_REG_BACKBUF] = (uint32_t)VGA_FB1;
}

void vga_fill(uint8_t color) {
  uint8_t *p = draw_buf;
  for (uint32_t i = 0U; i < VGA_SIZE; i++) {
    p[i] = color;
  }
}

void vga_swap(void) {
  /* BACKBUF already points at draw_buf (set after the previous swap). */
  VGA_CTRL[VGA_REG_BUFFER] = 0U; /* request swap at next vsync */
  wait_swap();
  /* draw_buf is now the displayed frame; render into the other one. */
  draw_buf = (draw_buf == VGA_FB0) ? VGA_FB1 : VGA_FB0;
  VGA_CTRL[VGA_REG_BACKBUF] = (uint32_t)draw_buf;
}

uint8_t *vga_back(void) { return draw_buf; }
