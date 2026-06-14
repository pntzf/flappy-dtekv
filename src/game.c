#include "game.h"
#include "lib.h"
#include "vga.h"

#define BIRD_START_Y FIXED_FROM_INT(VGA_HEIGHT / 2)

static int32_t diff_speed(uint32_t score) {
  int32_t s = PIPE_SPEED + (int32_t)(score / 10U);
  return (s < PIPE_SPEED_MAX) ? s : PIPE_SPEED_MAX;
}

static int32_t diff_gap(uint32_t score) {
  int32_t gap = PIPE_GAP_H - (int32_t)(score / 10U) * 10;
  return (gap > PIPE_GAP_MIN) ? gap : PIPE_GAP_MIN;
}

static pipe_t make_pipe(int32_t x, int32_t gap_h) {
  int32_t gap_min = gap_h / 2 + PIPE_GAP_MARGIN;
  int32_t gap_max =
      (int32_t)VGA_HEIGHT - GROUND_H - gap_h / 2 - PIPE_GAP_MARGIN;
  return (pipe_t){x, (int32_t)rand_range((uint32_t)gap_min, (uint32_t)gap_max),
                  gap_h, FALSE};
}

static pipe_t update_pipe(pipe_t p, int32_t rightmost, uint32_t *score,
                          uint32_t *flash, int32_t speed, int32_t gap_h) {
  p.x -= speed;

  if (!p.scored && (p.x + PIPE_W < BIRD_START_X)) {
    p.scored = TRUE;
    (*score)++;
    *flash = 4;
  }

  if (p.x + PIPE_W < 0) {
    p = make_pipe(rightmost + PIPE_SPACING, gap_h);
  }

  return p;
}

static int32_t bird_top(fixed_t bird_y) {
  return FIXED_TO_INT(bird_y) + BIRD_HITBOX_INSET;
}
static int32_t bird_bottom(fixed_t bird_y) {
  return FIXED_TO_INT(bird_y) + BIRD_H - BIRD_HITBOX_INSET;
}

/* triangle wave over 40 frames: phase 0->20->0, halved and centered -> bird
 * bobs +-5px */
static fixed_t bird_bob(uint32_t frame) {
  int32_t phase = (int32_t)(frame % 40);
  int32_t offset = (phase < 20 ? phase : 40 - phase) / 2 - 4;
  return BIRD_START_Y + FIXED_FROM_INT(offset);
}

static bool_t pipe_collides(const pipe_t *p, fixed_t bird_y) {
  int32_t gap_top = p->gap_y - p->gap_h / 2;
  int32_t gap_bottom = p->gap_y + p->gap_h / 2;

  if ((BIRD_START_X + BIRD_W - BIRD_HITBOX_INSET) < p->x ||
      (BIRD_START_X + BIRD_HITBOX_INSET) > p->x + PIPE_W) {
    return FALSE;
  }

  return (bird_top(bird_y) < gap_top) || (bird_bottom(bird_y) > gap_bottom);
}

static bool_t game_collision(const game_t *g) {
  fixed_t bird_y = g->bird.y;

  if (bird_bottom(bird_y) >= (int32_t)VGA_HEIGHT - GROUND_H) {
    return TRUE;
  }
  if (bird_top(bird_y) < 0) {
    return TRUE;
  }

  for (int32_t i = 0; i < MAX_PIPES; i++) {
    if (pipe_collides(&g->pipes[i], bird_y)) {
      return TRUE;
    }
  }

  return FALSE;
}

void reset_round(game_t *g) {
  g->score = 0;
  g->score_flash = 0;
  g->death_flash = 0;
  g->frame = 0;
  g->pipe_speed = PIPE_SPEED;
  g->bird.y = BIRD_START_Y;
  g->bird.vy = 0;

  for (int32_t i = 0; i < MAX_PIPES; i++) {
    g->pipes[i] = make_pipe((int32_t)VGA_WIDTH + i * PIPE_SPACING, PIPE_GAP_H);
  }
}

void game_return_to_menu(game_t *g) {
  reset_round(g);
  g->state = STATE_MENU;
}

void game_init(game_t *g) {
  g->high_score = 0;
  g->state = STATE_MENU;
  reset_round(g);
}

void game_start(game_t *g) {
  srand(g->frame);
  g->state = STATE_PLAYING;
  reset_round(g);
}

void game_flap(game_t *g) {
  if (g->state == STATE_PLAYING) {
    g->bird.vy = FLAP_VY;
  }
}

void game_update(game_t *g) {
  if (g->state != STATE_GAME_OVER) {
    g->frame++;
  }

  if (g->state == STATE_MENU) {
    g->bird.y = bird_bob(g->frame);
    return;
  }

  if (g->state == STATE_GAME_OVER) {
    if (g->death_flash > 0U) {
      g->death_flash--;
    }

    g->bird.vy += GRAVITY;
    g->bird.y += g->bird.vy;

    fixed_t ground_y = FIXED_FROM_INT((int32_t)VGA_HEIGHT - GROUND_H - BIRD_H);
    if (g->bird.y > ground_y) {
      g->bird.y = ground_y;
      g->bird.vy = 0;
    }
    return;
  }

  /* STATE_PLAYING */
  int32_t speed = diff_speed(g->score);
  int32_t gap_h = diff_gap(g->score);
  g->pipe_speed = speed;

  g->bird.vy += GRAVITY;
  g->bird.y += g->bird.vy;

  int32_t rightmost = 0;
  for (int32_t i = 0; i < MAX_PIPES; i++) {
    if (g->pipes[i].x > rightmost) {
      rightmost = g->pipes[i].x;
    }
  }

  if (g->score_flash > 0U) {
    g->score_flash--;
  }
  for (int32_t i = 0; i < MAX_PIPES; i++) {
    g->pipes[i] = update_pipe(g->pipes[i], rightmost, &g->score,
                              &g->score_flash, speed, gap_h);
  }

  if (game_collision(g)) {
    if (g->score > g->high_score) {
      g->high_score = g->score;
    }
    g->death_flash = 6;
    g->state = STATE_GAME_OVER;
  }
}
