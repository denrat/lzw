#include "lzw.h"
#include "stack.h"

void
encode_triple(triple t, int x, int y)
{
    static int mask_x0 = 0b111111110000;
    static int mask_x1 = 0b000000001111;

    // Store 8 left-most bits from x in the first byte of the triple
    t[0] = (x & mask_x0) >> 4;
    // Store 4 right-most bits from x
    // in the 4 left-most bits of the middle of the triple
    t[1] = (x & mask_x1) << 4;

    static int mask_y0 = 0b111100000000;
    static int mask_y1 = 0b000011111111;

    // Store 4 left-most bits from y
    // in the 4 left-most bits of the middle of the triple
    t[1] |= (y & mask_y0) >> 8;
    // Store 8 right-most bits from y in the last byte of the triple
    t[2] = y & mask_y1;
}

void
decode_triple(triple t, int *x, int *y)
{
    *x = (t[0] << 4) | ((t[1] & 0xF0) >> 4);
    *y = t[2] | ((t[1] << 8) & 0xF00);
}

int
search_dict(dictionary *dict, char *el, int length)
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
add_dict_entry(dictionary *dict, char s[], int length)
{
    char *entry = malloc(length * sizeof(char));
    strncpy(entry, s, length);
    printf("%d\t\"%s\"\n", dict->size + 256, s);

    dict->items[dict->size] = entry;
    dict->items_lengths[dict->size] = length;

    dict->size += 1;
}

dictionary *
create_dict(void)
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
        s[++length] = '\0';

        int index = search_dict(dict, s, length);

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

    stack st = NULL;

    dictionary *dict = create_dict();

    size_t nbread;

    while (nbread = fread(t, sizeof(triple), 1, src), nbread > 0)
    {
        // Put read values in the stack
        decode_triple(t, &twoints[0], &twoints[1]);
        push(twoints[1], &st);
        push(twoints[0], &st);

        printf("[+] Decoding triple: %d %d\n", twoints[0], twoints[1]);

        // Get values from the stack as long as they exist
        while (st)
        {
            print_stack(st);
            unsigned char c;
            int v = pop(&st);
            printf("[*] Next value: %d\t\t", v);

            // Unpack dictionary values if v is not a char
            if (v > 255)
            {
                printf("\n\t-> Unpacking into ");
                assert(v - 256 < dict->size);
                int len = strlen(dict->items[v - 256]);

                // Place the characters
                while (--len > 0)
                {
                    printf("%d ", dict->items[v - 256][len]);
                    push((int)dict->items[v - 256][len], &st);
                }

                printf("%d ", dict->items[v - 256][0]);
                c = dict->items[v - 256][0];
            }
            else
            {
                c = v;
            }

            fputc(c, dst);
            printf("'%c'\n", c);

            s[length] = c;
            s[++length] = '\0';

            int index = search_dict(dict, s, length);

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

void
lzw_decode(FILE *dst, FILE *src)
{
    int c; // An integer so it handles EOF
    char s[BUFSIZ];
    int length = 0;

    dictionary *dict = create_dict();

    // Get then write decoded characters
    while (c = next_char_no_stack(src, dict), c != EOF)
    {
        fputc(c, dst);

        s[length] = c;
        s[++length] = '\0';

        int index = search_dict(dict, s, length);

        if (index == -1)
        {
            add_dict_entry(dict, s, length);

            s[0] = c;
            s[1] = '\0';
            length = 1;
        }
    }

    free_dict(&dict);
}

int // int because EOF is not a char
next_char(FILE *src, dictionary *dict)
{
    static stack st;
    static triple t;
    static int ints[2];
    static size_t nb_read;

    // Exhaust the stack first
    if (!is_empty(st)) return pop(&st);

    // Load next triple in file
    nb_read = fread(t, sizeof(triple), 1, src);

    // Nothing being read means the file is exhausted
    if (nb_read == 0) return EOF;

    // Read values in the triple
    decode_triple(t, &ints[0], &ints[1]);

    // Push values to the stack
    for (int i = 0; i < 2; i++)
    {
        int x = ints[i];

        // Check if value is a char or a dict key
        if (x > 255)
        {
            int index = x - 256;

            // Push all characters from the dict item
            for (int j = dict->items_lengths[index] - 1; j >= 0; j--)
            {
                push(dict->items[index][j], &st);
            }
        }
        else
        {
            // Push char to stack
            push(x, &st);
        }
    }

    return pop(&st);
}

int
next_char_no_stack(FILE *src, dictionary *dict)
{
    enum state { LZW_EMPTY, LZW_FROM_DICT, LZW_FROM_TRIPLE };
    static enum state current_state;

    static triple t;
    static int ints[2];
    static int i, j;
    static char *word;

    // Manage empty buffer
    if (current_state == LZW_EMPTY)
    {
        // Read from file
        int nbread = fread(t, sizeof(triple), 1, src);

        // Check reading success
        if (nbread == 0)
        {
            return EOF;
        }

        i = 0;
        decode_triple(t, &ints[0], &ints[1]);
        current_state = LZW_FROM_TRIPLE;
    }

    // Read the triple decoded from src
    if (current_state == LZW_FROM_TRIPLE)
    {
        // Check ints[i] is a character
        if (ints[i] < 256)
        {
            // Get ready to read next triple if necessary
            if (i == 1)
            {
                current_state = LZW_EMPTY;
            }

            /* printf("%d", ints[i]); */
            return ints[i++];
        }

        // ints[i] is a dictionary key
        /* printf("dict->%d", ints[i]); */
        word = dict->items[ints[i++] - 256];
        current_state = LZW_FROM_DICT;
    }

    // Read characters from a dictionary word
    if (current_state == LZW_FROM_DICT)
    {
        // Check if the end of the word is reached
        if (j < dict->items_lengths[ints[i]])
        {
            return word[j++];
        }
        else
        {
            j = 0;
            current_state = (i < 2) ? LZW_FROM_TRIPLE : LZW_EMPTY;
        }
    }
}
