#ifndef _LZW_H
#define _LZW_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "global.h"

#define DOUZE 0b111111111111

typedef unsigned char triple[3];

typedef struct {
    char *items[DOUZE - 255];
    int items_lengths[DOUZE - 255];
    int size;
} dictionary;

// "Atomic" procedures
void encode_triple(triple, int, int);
void decode_triple(triple, int *, int *);

// Dictionary encoding
bool in_dict(dictionary, int, char *);
void emit_value(int [], int *, int, FILE *, FILE *);

// Glue functions
void encode_lzw(FILE *, FILE *);
void decode_lzw(FILE *, FILE *);

void lzw_decode(FILE *, FILE *);

int next_char(FILE *, dictionary *);
int next_char_no_stack(FILE *, dictionary *);

#endif
