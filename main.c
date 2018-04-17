#include "global.h"
#include "lzw.h"

#define DOUZE 0b111111111111

int
main(int argc, char *argv[])
{
    assert(argc == 3);

    FILE *src = NULL, *dst = NULL;

    if (!strcmp("e", argv[1]) || !strcmp("encode", argv[1]))
    {
        src = fopen(argv[2], "r");
        dst = fopen(strcat(argv[2], ".lzw"), "w");
        encode_lzw(dst, src);
    }
    else if (!strcmp("d", argv[1]) || !strcmp("decode", argv[1]))
    {
        src = fopen(argv[2], "r");
        dst = fopen(strcat(argv[2], ".unlzw"), "w");
        decode_lzw(dst, src);
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
