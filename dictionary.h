#ifndef _DICTIONARY_H
#define _DICTIONARY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "triple.h"

/**
 * dictionary.h
 * A dictionary is a structure made to store repetitive character sequences in the encoded text and their length.
 */

#define DOUZE 0b111111111111
#define DICT_MAX_SIZE (DOUZE - 255)

typedef struct {
    char *items[DICT_MAX_SIZE];
    int items_lengths[DICT_MAX_SIZE];
    int size;
} dictionary;

int dict_search(dictionary *, char [], int);
void dict_add_entry(dictionary *, char [], int);
void dict_init(dictionary *);
void dict_free(dictionary *);

#endif /* _DICTIONARY_H */
