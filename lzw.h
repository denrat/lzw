#ifndef _LZW_H
#define _LZW_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "triple.h"
#include "dictionary.h"

#define LZW_TRAILING 0

void encode_lzw(FILE *, FILE *);
void decode_lzw(FILE *, FILE *);

void lzw_encode(FILE *, FILE *);
void lzw_decode(FILE *, FILE *);

void lzw_decode_no_compression(FILE *, FILE *);
void lzw_encode_no_compression(FILE *, FILE *);

int next_char(FILE *, dictionary *);
int next_char_no_stack(FILE *, dictionary *);
int next_char_stream(FILE *, dictionary *);

#endif
