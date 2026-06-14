#ifndef LIB_H
#define LIB_H

#include "types.h"

void srand(uint32_t seed);
uint32_t rand(void);
uint32_t rand_range(uint32_t lo, uint32_t hi);

#endif /* LIB_H */