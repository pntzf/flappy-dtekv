#include "render.h"
#include "game.h"
#include "sprite.h"
#include "vga.h"

#include "bg.h"
#include "bird_sheet.h"
#include "digits.h"
#include "gameover.h"
#include "ground.h"
#include "message.h"
#include "pipe_green.h"
#include "pipe_red.h"

#define BIRD_ANIM_TICKS 6

#define SCORE_Y 8
#define DIGIT_GAP 2

#define MESSAGE_X ((VGA_WIDTH - MESSAGE_SPRITE_W) / 2)
#define MESSAGE_Y ((VGA_HEIGHT - MESSAGE_SPRITE_H) / 2)

#define GAMEOVER_X ((VGA_WIDTH - GAMEOVER_SPRITE_W) / 2)
#define GAMEOVER_Y ((VGA_HEIGHT / 2) - GAMEOVER_SPRITE_H - 8)
#define GAMEOVER_SCORE_Y (GAMEOVER_Y + GAMEOVER_SPRITE_H + 10)
#define GAMEOVER_BEST_Y (GAMEOVER_SCORE_Y + DIGIT_H + 8)

static void draw_number_centered(int32_t y, uint32_t n) {
  uint8_t digits[10];
  int32_t nd = 0;
  uint32_t tmp = n;

  do {
    digits[nd++] = (uint8_t)(tmp % 10U);
    tmp /= 10U;
  } while (tmp > 0U);

  int32_t total_w = nd * DIGIT_W + (nd - 1) * DIGIT_GAP;
  int32_t x = (VGA_WIDTH - total_w) / 2;

  for (int32_t i = nd - 1; i >= 0; i--, x += DIGIT_W + DIGIT_GAP) {
    sprite_draw(x, y, &digit_sprites[digits[i]]);
  }
}

static void draw_background(const game_t *g) {
  int32_t scroll = (int32_t)((g->frame * (uint32_t)g->pipe_speed) %
                             (uint32_t)GROUND_SPRITE_W);

  sprite_draw(0, 0, &bg_sprite);

  sprite_draw(-scroll, VGA_HEIGHT - GROUND_SPRITE_H, &ground_sprite);
  sprite_draw(GROUND_SPRITE_W - scroll, VGA_HEIGHT - GROUND_SPRITE_H,
              &ground_sprite);
}

static void draw_bird(const game_t *g) {
  int32_t anim_frame = (int32_t)((g->frame / (uint32_t)BIRD_ANIM_TICKS) % 3U);
  sprite_draw(BIRD_START_X, FIXED_TO_INT(g->bird.y),
              &bird_sprites[BIRD_COLOR_YELLOW][anim_frame]);
}

static void draw_pipe(const pipe_t *p, const game_t *g) {
  int32_t gap_top = p->gap_y - p->gap_h / 2;
  int32_t gap_bottom = p->gap_y + p->gap_h / 2;
  int32_t bottom_h = (int32_t)VGA_HEIGHT - gap_bottom - GROUND_H;

  if (bottom_h <= 0) {
    return;
  }

  /* Pipes are red in high score territory */
  const sprite_t *pipe =
      ((g->state == STATE_PLAYING) && (g->score > g->high_score))
          ? &pipe_red_sprite
          : &pipe_sprite;

  sprite_draw_flipped(p->x, gap_top - PIPE_SPRITE_H, pipe);
  sprite_draw_clipped(p->x, gap_bottom, pipe, bottom_h);
}

static void draw_score(const game_t *g) {
  if (g->score_flash > 2U) {
    return;
  }
  draw_number_centered(SCORE_Y, g->score);
}

static void draw_gameover(const game_t *g) {
  sprite_draw(GAMEOVER_X, GAMEOVER_Y, &gameover_sprite);
  draw_number_centered(GAMEOVER_SCORE_Y, g->score);
  draw_number_centered(GAMEOVER_BEST_Y, g->high_score);
}

void render(const game_t *g) {
  if (g->death_flash > 0U) {
    vga_fill(VGA_WHITE);
    return;
  }

  draw_background(g);

  switch (g->state) {
  case STATE_MENU:
    draw_bird(g);
    sprite_draw(MESSAGE_X, MESSAGE_Y, &message_sprite);
    break;

  case STATE_PLAYING:
    for (int32_t i = 0; i < MAX_PIPES; i++) {
      draw_pipe(&g->pipes[i], g);
    }
    draw_bird(g);
    draw_score(g);
    break;

  case STATE_GAME_OVER:
    for (int32_t i = 0; i < MAX_PIPES; i++) {
      draw_pipe(&g->pipes[i], g);
    }
    draw_bird(g);
    draw_gameover(g);
    break;

  default:
    break;
  }
}
