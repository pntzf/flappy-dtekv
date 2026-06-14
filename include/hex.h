#ifndef HEX_H
#define HEX_H

#include "types.h"

void hex_write(uint8_t display, uint8_t val);
void hex_clear(void);
void hex_write_score(uint32_t score, uint32_t high_score);

#endif /* HEX_H */