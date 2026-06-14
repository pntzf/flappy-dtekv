#include "dtekv-lib.h"
#include "dtekvio.h"

#define JTAG_UART ((volatile uint32_t *)MMIO_JTAG_UART)
#define JTAG_CTRL ((volatile uint32_t *)MMIO_JTAG_CTRL)

void printc(char c) {
  while ((*JTAG_CTRL & 0xFFFF0000U) == 0U) {
    ;
  }
  *JTAG_UART = (uint32_t)c;
}

/* Non-blocking variant — drops the character if JTAG isn't ready.
 * Use in ISR-sensitive paths where blocking would miss a frame. */
bool_t printc_try(char c) {
  if ((*JTAG_CTRL & 0xFFFF0000U) == 0U) {
    return FALSE;
  }
  *JTAG_UART = (uint32_t)c;
  return TRUE;
}

void print(const char *s) {
  while (*s != '\0') {
    printc(*s++);
  }
}

/* Same as print() but skips characters when JTAG buffer is full.
 * Returns FALSE if any character was dropped, TRUE if all printed. */
bool_t print_try(const char *s) {
  bool_t all_printed = TRUE;
  while (*s != '\0') {
    if (!printc_try(*s)) {
      all_printed = FALSE;
    }
    s++;
  }
  return all_printed;
}

void print_dec(uint32_t x) {
  uint32_t divisor = 1000000000U;
  bool_t first = FALSE;
  do {
    uint32_t d = x / divisor;
    if (d != 0U) {
      first = TRUE;
    }
    if (first != FALSE) {
      printc((char)('0' + d));
    }
    x -= d * divisor;
    divisor /= 10U;
  } while (divisor != 0U);
  if (first == FALSE) {
    printc('0');
  }
}

void print_dec_try(uint32_t x) {
  uint32_t divisor = 1000000000U;
  bool_t first = FALSE;
  do {
    uint32_t d = x / divisor;
    if (d != 0U) {
      first = TRUE;
    }
    if (first != FALSE) {
      printc_try((char)('0' + d));
    }
    x -= d * divisor;
    divisor /= 10U;
  } while (divisor != 0U);
  if (first == FALSE) {
    printc_try('0');
  }
}

void print_hex32(uint32_t x) {
  printc('0');
  printc('x');
  for (int32_t i = 7; i >= 0; i--) {
    char hd = (char)((x >> ((uint32_t)i * 4U)) & 0xFU);
    printc((char)(hd < (char)10 ? hd + '0' : hd + ('A' - (char)10)));
  }
}

void handle_exception(uint32_t arg0, uint32_t arg1, uint32_t arg2,
                      uint32_t arg3, uint32_t arg4, uint32_t arg5,
                      uint32_t mcause, uint32_t syscall_num) {
  switch (mcause) {
  case 0U:
    print("\n[EXCEPTION] Instruction address misalignment. ");
    break;
  case 2U:
    print("\n[EXCEPTION] Illegal instruction. ");
    break;
  case 11U:
    if (syscall_num == 4U) {
      print((const char *)arg0);
    }
    if (syscall_num == 11U) {
      printc((char)arg0);
    }
    return;
  default:
    print("\n[EXCEPTION] Unknown error. ");
    break;
  }

  print("Exception Address: ");
  print_hex32(arg0);
  printc('\n');
  for (;;) {
    ;
  }
}
