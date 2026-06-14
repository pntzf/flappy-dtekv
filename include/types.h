#ifndef TYPES_H
#define TYPES_H

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned int size_t;

typedef uint8_t bool_t;
#define TRUE (1U)
#define FALSE (0U)

#ifndef NULL
#define NULL ((void *)0)
#endif

#endif /* TYPES_H */