#ifndef _LZW_H
#define _LZW_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "triple.h"
#include "dictionary.h"

#define LZW_TRAILING 0

void lzw_encode(FILE *, FILE *);
void lzw_decode(FILE *, FILE *);

void lzw_encode_no_compression(FILE *, FILE *);

int next_char(FILE *, dictionary *);

#endif
