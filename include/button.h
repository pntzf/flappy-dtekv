#ifndef BUTTON_H
#define BUTTON_H

#include "types.h"

void button_init(void);
bool_t button_read(void);
bool_t button_was_pressed(void);

#endif /* BUTTON_H */