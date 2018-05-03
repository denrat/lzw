#include <assert.h>

#include "lzw.h"

#include "tools.h"
#include "stack.h"
#include "triple.h"
#include "dictionary.h"

void
lzw_encode(FILE *dst, FILE *src)
{
    char s[BUFSIZ] = "";
    int c = 0; // An int so it handles EOF
    int length = 0;
    off_t src_size = 0;

    int last_index = -1;

    dictionary *dict = dict_create();

    // Consider no trailing value
    fputc(0, dst);

    // TODO eventually use _unlocked variants of fgetc
    while (c = fgetc(src), c != EOF)
    {
        // Move next char to end of string
        fputc(c, stdout);
        src_size++;

        s[length] = (char)c;
        s[++length] = '\0';

        int index = dict_search(dict, s, length);

        if (index == -1)
        {
            // Emit code of known string or char
            emit_code(dst, src, last_index);

            // Add to dict
            dict_add_entry(dict, s, length);

            // Reset char sequence
            s[0] = (char)c;
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

    // Add trailing character if necessary
    if (src_size % 2 == 1)
    {
        emit_code(dst, src, 0);

        // Update dst to show there is a trailing zero
        rewind(dst);
        fputc(1, dst);
    }

    dict_free(&dict);
}

void
lzw_encode_no_compression(FILE *dst, FILE *src)
{
    //
    // Let's try with compression but no dictionary
    //

    // An int char so it handles EOF (i.e. -1)
    int c = 0;

    off_t src_size = 0;

    // Consider no trailing value
    fputc(0, dst);

    // TODO eventually use _unlocked variants of fgetc
    while (c = fgetc(src), c != EOF)
    {
        fputc(c, stdout);
        emit_code(dst, src, c);

        src_size++;
    }

    // Add trailing character if necessary
    if (src_size % 2 == 1)
    {
        emit_code(dst, src, 0);

        // Update dst to show there is a trailing zero
        rewind(dst);
        fputc(1, dst);
    }
}

void
decode_lzw(FILE *dst, FILE *src)
{
    char s[BUFSIZ] = "";
    int length = 0;

    triple t;
    int twoints[2];

    stack st = NULL;

    dictionary *dict = dict_create();

    size_t nbread;

    while (nbread = fread(t, sizeof(triple), 1, src), nbread > 0)
    {
        // Put read values in the stack
        triple_decode(t, &twoints[0], &twoints[1]);
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

            int index = dict_search(dict, s, length);

            if (index == -1)
            {
                dict_add_entry(dict, s, length);

                s[0] = c;
                s[1] = 0;
                length = 1;
            }
        }
    }

    dict_free(&dict);
}

void
lzw_decode(FILE *dst, FILE *src)
{
    int c; // An integer so it handles EOF
    char s[BUFSIZ];
    int length = 0;

    dictionary *dict = dict_create();

    // Get then write decoded characters
    while (c = next_char_stream(src, dict), c != EOF)
    {
        fputc(c, dst);

        s[length] = c;
        s[++length] = '\0';

        int index = dict_search(dict, s, length);

        if (index == -1)
        {
            dict_add_entry(dict, s, length);

            s[0] = c;
            s[1] = '\0';
            length = 1;
        }
    }

    dict_free(&dict);
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
    triple_decode(t, &ints[0], &ints[1]);

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
        triple_decode(t, &ints[0], &ints[1]);
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

    // Nothing has been returned, force next iteration
    return next_char_no_stack(src, dict);
}

int
next_char_stream(FILE *src, dictionary *dict)
{
    // Unpack values from triple (and eventually dictionary) and push them to src
    static int has_trailing = -1;
    static int queued;
    static triple t;
    static int tbuf[2];

    // Exhaust previously pushed characters
    if (queued) goto exhaust;

    // First iteration: determine whether to expect a trailing zero
    // TODO maybe append raw character to file instead: fread and feof will allow to test trailing characters presence anyway
    if (has_trailing < 0) has_trailing = fgetc(src);

    // Try to read triple from file
    if (!fread(t, sizeof(triple), 1, src)) return EOF;
    triple_decode(t, &tbuf[0], &tbuf[1]);

    // Unpack values from buffer
    for (int i = 1; i >= 0; i--)
    {
        int v = tbuf[i];

        // Push back character(s) to stream
        if (v == LZW_TRAILING && feof(src) && i == 1 && has_trailing)
        {
            // Discard trailing value
            continue;
        }
        else if (v < 256)
        {
            // Push simple character to the stream
            queued++;
            ungetc(v, src);
        }
        else
        {
            // Read dictionary value and push it back in the stream
            int dkey = v - 256;
            for (int j = dict->items_lengths[dkey]; j >= 0; j--)
            {
                queued++;
                ungetc(dict->items[dkey][j], src);
            }
        }
    }

exhaust:
    queued--;
    return fgetc(src);
}
