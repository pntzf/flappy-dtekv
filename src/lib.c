#include "lib.h"

static uint32_t rand_state = 12345U;

void srand(uint32_t seed) {
  rand_state = seed ^ 12345U; /* xorshift32 must not start at 0 */
  if (rand_state == 0) {
    rand_state = 12345U;
  }
}

/* Xorshift32 RNG, period 2^32-1 */
uint32_t rand(void) {
  rand_state ^= rand_state << 13U;
  rand_state ^= rand_state >> 17U;
  rand_state ^= rand_state << 5U;
  return rand_state;
}
/* Return a random integer in the range [lo, hi) */
uint32_t rand_range(uint32_t lo, uint32_t hi) {
  if (hi <= lo) {
    return lo;
  }
  return lo + (rand() % (hi - lo));
}
