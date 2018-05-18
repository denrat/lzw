#include <assert.h>

#include "lzw.h"

#include "tools.h"
#include "triple.h"
#include "dictionary.h"

void
lzw_encode(FILE *dst, FILE *src)
{
    int c; // An integer so it handles EOF

    // A string/buffer to contain the dictionary values
    char s[BUFSIZ];
    int length = 0;

    dictionary dict;
    dict_init(&dict);

    // Keep track of the dictionary codes
    int code, prev_code;

    // Keep track of trailing character
    bool has_trailing = false;

#ifdef DEBUG
    long long emitted = 0;
#endif
    // Read byte by byte
    while (c = fgetc(src), c != EOF)
    {
        // Add character to buffer
        s[length] = c;
        s[++length] = '\0';

        // Dictionary lookup
        code = dict_search(&dict, s, length);

        if (code == DICT_NOT_FOUND)
        {
            // Prepare code for packing
            emit_code(dst, prev_code);

#ifdef DEBUG
            emitted++;
#endif

            // Add new value to dictionary
            dict_add_entry(&dict, s, length);

            // Reset buffer
            s[0] = c;
            s[1] = '\0';
            length = 1;

            prev_code = (int)c;
        }
        else
        {
            prev_code = code;
        }

        has_trailing ^= 1; // Negate the value
    }

    if (has_trailing)
    {
        // `emit_code' won't have enough codes to append it to file
        // Append raw code as a short
        short shortcode = prev_code;
        fwrite(&shortcode, sizeof(short), 1, dst);
    }
    else
    {
        // Even number of codes, emit prev_code
        emit_code(dst, prev_code);
    }

#ifdef DEBUG
    printf("%lld emitted\n", emitted);
#endif

    dict_free(&dict);
}

void
lzw_decode(FILE *dst, FILE *src)
{
    int c; // An integer so it handles EOF

    char s[BUFSIZ];
    int length = 0;

    int code;
    dictionary dict;
    dict_init(&dict);

    // Get then write decoded characters
    while (c = next_char(src, &dict), c != EOF)
    {
        fputc(c, dst);

        s[length] = c;
        s[++length] = '\0';

        code = dict_search(&dict, s, length);

        if (code == DICT_NOT_FOUND)
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
    static int queued;
    static triple t;
    static int buf[2];
    static int bufpos = 2, bufpos_max = 1;
    static int v;
    static size_t trsize = sizeof(triple);
    static size_t nbread;

    if (queued) goto exhaust;
    if (bufpos <= bufpos_max) goto read_code;

    nbread = fread(t, 1, trsize, src);
    if (nbread == trsize) // Full triple read
    {
        triple_decode(t, &buf[0], &buf[1]);
        bufpos = 0;
    }
    else if (nbread > 0) // Not enough bytes for a triple
    {
        short s;

        // Read the misread value
        fseek(src, (long)(-nbread), SEEK_CUR);
        fread(&s, nbread, 1, src);

        // Prepare buffer for reading
        buf[0] = s;

        // Restrain reading of buffer
        bufpos = 0;
        bufpos_max--;
    }
    else // Nothing left to read
    {
        return EOF;
    }

read_code:
    v = buf[bufpos++];

    if (v < 256)
    {
        return v;
    }
    else
    {
        int dkey = v - 256;
        for (int i = dict->items_lengths[dkey] - 1; i > 0; i--)
        {
            queued++;
            ungetc(dict->items[dkey][i], src);
        }

        return dict->items[dkey][0];
    }

exhaust:
    queued--;
    return fgetc(src);
}
