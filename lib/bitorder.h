/* Bit ordering detection library:
   The code in this file is provided free of copyright. It may be used freely and without constraint.
   In fact, I copied it from a stackoverflow thread, so I couldn't claim intellectual property rights
   to it even if I wanted to.
   Last updated: 2024 June
 */

#ifndef BITORDER_H
#define BITORDER_H


#include <limits.h>
#include <stdint.h>

#if CHAR_BIT != 8
#error "unsupported char size"
#endif

enum
{
    O32_LITTLE_ENDIAN = 0x03020100ul,
    O32_BIG_ENDIAN = 0x00010203ul,
    O32_PDP_ENDIAN = 0x01000302ul,      /* DEC PDP-11 (aka ENDIAN_LITTLE_WORD) */
    O32_HONEYWELL_ENDIAN = 0x02030001ul /* Honeywell 316 (aka ENDIAN_BIG_WORD) */
};

static const union { unsigned char bytes[4]; uint32_t value; } o32_host_order =
    { { 0, 1, 2, 3 } };

#define O32_HOST_ORDER (o32_host_order.value)


#endif //BITORDER_H
