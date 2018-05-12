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

    // Keep track of the dictionary indexes
    int index, prev_index;

    // Keep track of trailing character
    bool has_trailing = false;

    // Read byte by byte
    while (c = fgetc(src), c != EOF)
    {
        // Add character to buffer
        s[length] = c;
        s[++length] = '\0';

        // Dictionary lookup
        index = dict_search(&dict, s, length);

        if (index == DICT_NOT_FOUND)
        {
            // Prepare code for packing
            emit_code(dst, src, prev_index);

            // Add new (unknown) value to dictionary
            dict_add_entry(&dict, s, length);

            // Reset buffer
            s[0] = c;
            s[1] = '\0';
            length = 1;

            prev_index = (int)c;
        }
        else
        {
            prev_index = index;
        }

        has_trailing ^= 1; // Negate the value
    }

    if (has_trailing)
    {
        // `emit_code' won't have enough codes to append it to file
        // Append raw code as a short
        short shortcode = prev_index;
        fwrite(&shortcode, sizeof(short), 1, dst);
    }
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
    // Unpack values from triple (and possibly dictionary) and push them to src
    static int queued;
    static triple t;
    static int buf[2];
    static int imax = 1;
    static size_t trsize = sizeof(triple);

    // Exhaust previously pushed characters
    if (queued) goto exhaust;

    // Read triple or consider it as trailing values
    size_t nbread = fread(t, 1, trsize, src);
    if (nbread == trsize)
    {
        // Unpack triple into the buffer
        triple_decode(t, &buf[0], &buf[1]);
    }
    else if (nbread > 0)
    {
        // Not enough values to constitute a triple have been read
        short s;

        // Read the misread value
        fseek(src, (long)(-nbread), SEEK_CUR);
        fread(&s, nbread, 1, src);

        // Prepare buffer for reading
        buf[0] = s;

        // Restrain reading of buffer
        imax--;
    }
    else
    {
        // No more values to read
        return EOF;
    }

    // Unpack values from buffer
    for (int i = imax; i >= 0; i--)
    {
        int v = buf[i];

        if (v < 256)
        {
            // Push simple character to the stream
            queued++;
            ungetc(v, src);
        }
        else
        {
            // Read dictionary value and push it back in the stream
            int dkey = v - 256;
            for (int j = dict->items_lengths[dkey] - 1; j >= 0; j--)
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
