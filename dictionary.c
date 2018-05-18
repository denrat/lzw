#include <stdbool.h>

#include "dictionary.h"

int
dict_search(dictionary *dict, char el[], int length)
{
    static int index;

    // Check if string only has one character
    if (el[1] == '\0')
    {
        return (unsigned char)el[0];
    }

    // Compare provided word to dict words
    for (index = 0; index < dict->size; index++)
    {
        if (length == dict->items_lengths[index]
                && !strncmp(el, dict->items[index], length))
        {
            return index + 256;
        }
    }

    return DICT_NOT_FOUND;
}

void
dict_add_entry(dictionary *dict, char s[], int length)
{
    static bool reset = false;
    // FIXME
    if (dict->size == DICT_MAX_SIZE)
    {
        reset = true;
        dict->size /= 2;
    }

    char *entry = malloc(length * sizeof(char));
    strncpy(entry, s, length);

#ifdef DEBUG
    printf("%d\t\"%s\"\t(%d)\n", dict->size + 256, s, length);
#endif

    if (reset) free(dict->items[dict->size]);
    dict->items[dict->size] = entry;
    dict->items_lengths[dict->size] = length;

    dict->size += 1;
}

void
dict_init(dictionary *dict)
{
    dict->size = 0;
    dict->full = 0;
    for (int i = 0; i < DICT_MAX_SIZE; i++)
    {
        dict->items[i] = NULL;
        dict->items_lengths[i] = 0;
    }
}

void
dict_free(dictionary *dict)
{
#ifdef DEBUG
    printf("\n%d entries\n", dict->size);
#endif

    for (int i = 0; i < DICT_MAX_SIZE; i++)
    {
        if (dict->items[i]) free(dict->items[i]);
    }
}
