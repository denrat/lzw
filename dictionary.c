#include "dictionary.h"

int
dict_search(dictionary *dict, char el[], int length)
{
    static int index;

    // Check if string only has one character
    if (el[1] == 0)
    {
        return (int)el[0];
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

    return -1; // Not found
}

void
dict_add_entry(dictionary *dict, char s[], int length)
{
    char *entry = malloc(length * sizeof(char));
    strncpy(entry, s, length);
    printf("%d\t\"%s\"\n", dict->size + 256, s);

    dict->items[dict->size] = entry;
    dict->items_lengths[dict->size] = length;

    dict->size += 1;
}

void
dict_init(dictionary *dict)
{
    dict->size = 0;
}

void
dict_free(dictionary *dict)
{
    printf("\n%d entries\n", dict->size);

    for (int i = 0; i < dict->size; i++)
    {
        free(dict->items[i]);
    }
}
