#ifndef DTEKV_LIB_H
#define DTEKV_LIB_H

#include "types.h"

void printc(char c);
void print(const char *s);
void print_dec(uint32_t x);
extern bool_t printc_try(char c);
extern bool_t print_try(const char *s);
void print_dec_try(uint32_t x);
void print_hex32(uint32_t x);

void handle_exception(uint32_t arg0, uint32_t arg1, uint32_t arg2,
                      uint32_t arg3, uint32_t arg4, uint32_t arg5,
                      uint32_t mcause, uint32_t syscall_num);

#endif /* DTEKV_LIB_H */
