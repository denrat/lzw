#include "lzw.h"
#include "stack.h"

void
encode_triple(triple t, const int x, const int y)
{
    const static int mask_x0 = 0b111111110000;
    const static int mask_x1 = 0b000000001111;

    // Store 8 left-most bits from x in the first byte of the triple
    t[0] = (x & mask_x0) >> 4;
    // Store 4 right-most bits from x
    // in the 4 left-most bits of the middle of the triple
    t[1] = (x & mask_x1) << 4;

    const static int mask_y0 = 0b111100000000;
    const static int mask_y1 = 0b000011111111;

    // Store 4 left-most bits from y
    // in the 4 left-most bits of the middle of the triple
    t[1] |= (y & mask_y0) >> 8;
    // Store 8 right-most bits from y in the last byte of the triple
    t[2] = y & mask_y1;
}

void
decode_triple(const triple t, int *x, int *y)
{
    *x = (t[0] << 4) | ((t[1] & 0xF0) >> 4);
    *y = t[2] | ((t[1] << 8) & 0xF00);
}

int
search_dict(dictionary *dict, char *el)
{
    static int index;

    if (el[1] == 0)
    {
        return (int)el[0];
    }

    for (index = 0; index < dict->size; index++)
    {
        if (!strcmp(el, dict->items[index]))
        {
            return index + 256;
        }
    }

    return -1;
}

void
emit_code(FILE *dst, FILE *src, int code)
{
    static triple t;
    static int codebuf[2];
    static int count = 0;

    codebuf[count++] = code;

    if (feof(src) && count == 1)
    {
        // Add trailing zero
        codebuf[count++] = 0;
    }

    if (count == 2)
    {
        encode_triple(t, codebuf[0], codebuf[1]);
        fwrite(t, sizeof(triple), 1, dst);

        count = 0;
    }
}

void
add_dict_entry(dictionary *dict, const char s[], const int length)
{
    char *entry = malloc((length + 1) * sizeof(char));
    strcpy(entry, s);
    /* printf("%d: \"%s\"\n", dict->size, s); */

    dict->items[dict->size] = entry;

    dict->size += 1;
}

dictionary *
create_dict()
{
    dictionary *dict;
    dict = (dictionary *)malloc(sizeof(dictionary));
    dict->size = 0;

    return dict;
}

void
free_dict(dictionary **dict)
{
    printf("\n%d entries\n", (*dict)->size);

    for (int i = 0; i < (*dict)->size; i++)
    {
        free((*dict)->items[i]);
    }

    free(*dict);

    *dict = NULL;
}

void
encode_lzw(FILE *dst, FILE *src)
{
    char s[BUFSIZ] = "";
    char c = 0;
    int length = 0;

    int last_index = -1;

    dictionary *dict = create_dict();

    // TODO eventually use _unlocked variants of fgetc
    while (c = fgetc(src), c != EOF)
    {
        // Move next char to end of string
        fputc(c, stdout);
        s[length] = (char)c;
        s[length + 1] = '\0';

        int index = search_dict(dict, s);

        if (index == -1)
        {
            // Emit code of known string or char
            emit_code(dst, src, last_index);

            // Add to dict
            add_dict_entry(dict, s, length);

            // Reset char sequence
            s[0] = c;
            s[1] = '\0';
            length = 1;
            last_index = (int)c;
        }
        else
        {
            last_index = index;
            length++;
        }
    }

    emit_code(dst, src, last_index);

    free_dict(&dict);
}

void
decode_lzw(FILE *dst, FILE *src)
{
    char s[BUFSIZ] = "";
    int length = 0;

    triple t;
    int twoints[2];

    list ls = NULL;

    dictionary *dict = create_dict();

    size_t nbread;

    while (nbread = fread(t, sizeof(triple), 1, src), nbread > 0)
    {
        decode_triple(t, &twoints[0], &twoints[1]);

        push(twoints[1], &ls);
        push(twoints[0], &ls);

        while (ls)
        {
            int v = pop(&ls);
            printf("%d", v);

            if (v > 255)
            {
                assert(v - 256 < dict->size);
                int len = strlen(dict->items[v - 256]);

                while (--len > 0)
                {
                    push(dict->items[v - 256][len], &ls);
                }

                v = dict->items[v - 256][0];
            }

            char c = (char)v;

            fputc(c, dst);
            fputc(c, stdout);

            s[length] = (char)c;
            s[++length] = '\0';

            int index = search_dict(dict, s);

            if (index == -1)
            {
                add_dict_entry(dict, s, length);

                s[0] = c;
                s[1] = 0;
                length = 1;
            }
        }
    }

    free_dict(&dict);
}
