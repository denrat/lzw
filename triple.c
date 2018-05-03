#include "triple.h"

void
triple_encode(triple t, int x, int y)
{
    static int mask_x0 = 0b111111110000;
    static int mask_x1 = 0b000000001111;

    // Store 8 left-most bits from x in the first byte of the triple
    t[0] = (x & mask_x0) >> 4;
    // Store 4 right-most bits from x
    // in the 4 left-most bits of the middle of the triple
    t[1] = (x & mask_x1) << 4;

    static int mask_y0 = 0b111100000000;
    static int mask_y1 = 0b000011111111;

    // Store 4 left-most bits from y
    // in the 4 left-most bits of the middle of the triple
    t[1] |= (y & mask_y0) >> 8;
    // Store 8 right-most bits from y in the last byte of the triple
    t[2] = y & mask_y1;
}

void
triple_decode(triple t, int *x, int *y)
{
    *x = (t[0] << 4) | ((t[1] & 0xF0) >> 4);
    *y = t[2] | ((t[1] << 8) & 0xF00);
}
