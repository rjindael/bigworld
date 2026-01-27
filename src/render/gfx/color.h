#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

typedef struct {
    uint8_t r, g, b;
} sb_Color3;

typedef struct {
    uint8_t r, g, b, a;
} sb_Color4;

#endif // COLOR_H