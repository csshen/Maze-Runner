#include "math_extra.h"
#include <stdint.h>

int in_range(int x, int lower, int upper)
{
    return (x > lower && x < upper);
}

float clamp(float x, float limit)
{
    x = (x > limit) ? limit : x;
    x = (x < -limit) ? -limit : x;
    return x;
}

int sgn(float x)
{
    return (x > 0.0) - (x < 0.0);
}

float coin_flip()
{
    // Compute psuedorandom numbers! Don't worry about how this works.
    // If you really must know, search for "Linear-feedback shift register"
    static uint16_t rand = 0xA1B2;
    uint16_t bit = ((rand >> 0) ^ (rand >> 2) ^ (rand >> 3) ^ (rand >> 5) ) & 1;
    rand = (rand >> 1) | (bit << 15);
    return (rand & 1) ? 1.0 : -1.0;
}