#ifndef BIRD_SHEET_H
#define BIRD_SHEET_H

#include "sprite.h"
#include "types.h"

#define BIRD_FRAME_W 20
#define BIRD_FRAME_H 14
#define BIRD_FRAME_SIZE 280

/* [color][frame]: 0=red, 1=blue, 2=yellow */
#define BIRD_COLOR_RED    0
#define BIRD_COLOR_BLUE   1
#define BIRD_COLOR_YELLOW 2

extern const sprite_t bird_sprites[3][3];

#endif /* BIRD_SHEET_H */
