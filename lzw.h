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
    int size;
} dictionary;

// "Atomic" procedures
void encode_triple(triple, const int, const int);
void decode_triple(const triple, int *, int *);

// Dictionary encoding
bool in_dict(dictionary, int, char *);
void emit_value(int [], int *, int, FILE *, FILE *);

// Glue functions
void encode_lzw(FILE *, FILE *);
void decode_lzw(FILE *, FILE *);

#endif
