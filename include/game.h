#ifndef GAME_H
#define GAME_H

#include "types.h"

/* -------------------------------------------------------------------------
 * Fixed-point (Q16.16). The core has no FPU, so all physics uses scaled
 * integers: 1.0 px == FIXED_ONE. Add/compare are plain integer ops.
 * ------------------------------------------------------------------------- */
typedef int32_t fixed_t;
#define FIXED_SHIFT 16
#define FIXED_ONE (1 << FIXED_SHIFT)
#define FIXED_FROM_INT(i) ((fixed_t)(i) << FIXED_SHIFT)
#define FIXED_TO_INT(f)                                                        \
  ((int32_t)((f) >> FIXED_SHIFT)) /* floor toward -inf   */

#define GRAVITY (FIXED_ONE / 4)  /* 0.25 px/frame^2 */
#define FLAP_VY (-5 * FIXED_ONE) /* -5.0 px/frame   */

#define GROUND_H 40
#define BIRD_W 20
#define BIRD_H 14
#define BIRD_HITBOX_INSET 3
#define BIRD_START_X 60
#define PIPE_W 30

#define MAX_PIPES 3
#define PIPE_SPACING 200
#define PIPE_SPEED 3
#define PIPE_SPEED_MAX 8
#define PIPE_GAP_H 110
#define PIPE_GAP_MIN 60
#define PIPE_GAP_MARGIN 20

typedef enum : uint8_t {
  STATE_MENU,
  STATE_PLAYING,
  STATE_GAME_OVER
} game_state_t;

typedef struct {
  int32_t x;
  int32_t gap_y;
  int32_t gap_h;
  bool_t scored;
} pipe_t;

typedef struct {
  fixed_t y;
  fixed_t vy;
} bird_t;

typedef struct {
  game_state_t state;
  uint32_t score;
  uint32_t high_score;
  uint32_t frame;
  uint32_t score_flash;
  uint32_t death_flash;
  int32_t pipe_speed;
  bird_t bird;
  pipe_t pipes[MAX_PIPES];
} game_t;

/* -------------------------------------------------------------------------
 * Functions
 * ------------------------------------------------------------------------- */
void game_init(game_t *g);
void game_start(game_t *g);
void game_flap(game_t *g);
void game_update(game_t *g);
void game_return_to_menu(game_t *g);

#endif /* GAME_H */
