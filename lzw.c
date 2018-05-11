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

    int last_index = -1;

    dictionary dict;
    dict_init(&dict);

    while (c = fgetc(src), c != EOF)
    {
        // Move next char to end of string
        fputc(c, stdout);

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

    dict_free(&dict);
}

void
lzw_encode_no_compression(FILE *dst, FILE *src)
{
    //
    // Let's try with compression (=triple) but no dictionary
    //

    // An int char so it handles EOF (i.e. -1)
    int c;
    int prev_code;
    unsigned counter = 0;

    while (c = fgetc(src), c != EOF)
    {
        fputc(c, stdout);
        emit_code(dst, src, c);
        prev_code = c;

        counter++;
    }

    if (counter % 2 == 1)
    {
        // `emit_code' won't have enough codes to append it to file
        // Append raw code as a short
        short shortcode = prev_code;
        printf("Appending raw code %d\n", (int)shortcode);
        fwrite(&shortcode, sizeof(short), 1, dst);
        /* short shortcode = code; */
        /* fwrite(&shortcode, sizeof(short), 1, dst); */
    }
}

void
lzw_decode(FILE *dst, FILE *src)
{
    int c; // An integer so it handles EOF
    char s[BUFSIZ];
    int length = 0;

    dictionary dict;
    dict_init(&dict);

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
