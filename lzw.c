#include <assert.h>

#include "lzw.h"

#include "tools.h"
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

    dictionary dict;
    dict_init(&dict);

    // Consider no trailing value
    // TODO manage this another way
    fputc(0, dst);

    while (c = fgetc(src), c != EOF)
    {
        // Move next char to end of string
        fputc(c, stdout);
        src_size++;

        s[length] = (char)c;
        s[++length] = '\0';

        int index = dict_search(&dict, s, length);

        if (index == -1)
        {
            // Emit code of known string or char
            emit_code(dst, src, last_index);

            // Add to dict
            dict_add_entry(&dict, s, length);

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
lzw_decode(FILE *dst, FILE *src)
{
    int c; // An integer so it handles EOF
    char s[BUFSIZ];
    int length = 0;

    dictionary dict;
    dict_init(dict);

    // Get then write decoded characters
    while (c = next_char(src, &dict), c != EOF)
    {
        fputc(c, dst);

        s[length] = c;
        s[++length] = '\0';

        int index = dict_search(&dict, s, length);

        if (index == -1)
        {
            dict_add_entry(&dict, s, length);

            s[0] = c;
            s[1] = '\0';
            length = 1;
        }
    }

    dict_free(&dict);
}

int
next_char(FILE *src, dictionary *dict)
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
    // Send next character in the stream
    queued--;
    return fgetc(src);
}
