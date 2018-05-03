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

typedef struct {
    char *items[DOUZE - 255];
    int items_lengths[DOUZE - 255];
    int size;
} dictionary;

int dict_search(dictionary *, char [], int);
void dict_add_entry(dictionary *, char [], int);
dictionary *dict_create(void);
void dict_free(dictionary **);

#endif /* _DICTIONARY_H */
