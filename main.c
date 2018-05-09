#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "lzw.h"

#include "tools.h"

#define DOUZE 0b111111111111

// TODO allow several options, such as a verbose mode, inspired by `tar'

int
main(int argc, char *argv[])
{
    assert(argc == 3);

    FILE *dst, *src;

    if (!strcmp("e", argv[1]) || !strcmp("encode", argv[1]))
    {
        src = fopen(argv[2], "r");
        dst = fopen(strcat(argv[2], ".lzw"), "w");
        /* lzw_encode_no_compression(dst, src); */
        lzw_encode(dst, src);
    }
    else if (!strcmp("d", argv[1]) || !strcmp("decode", argv[1]))
    {
        src = fopen(argv[2], "r");
        dst = fopen(strcat(argv[2], ".unlzw"), "w");
        lzw_decode(dst, src);
    }
    else
    {
        puts("Invalid arguments.");
        puts("\tUsage: lzw COMMAND FILENAME");
        puts("\t\twhere COMMAND is encode (e) or decode (d)");

        return EXIT_FAILURE;
    }

    fclose(src);
    fclose(dst);

    return EXIT_SUCCESS;
}
