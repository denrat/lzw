#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "lzw.h"
#include "tools.h"
#include "arguments.h"

int
main(int argc, char *argv[])
{
    struct args args;
    FILE *dst, *src;

    // Parse cli arguments
    args = parse_args(argc, argv);

    // Set the output to stdout if required
    dst = (args.modes & STDOUT_MODE) ? stdout : fopen(args.output, "w");
    src = fopen(args.input, "r");

    // Decode or encode depending on the cli arguments
    if (args.modes & ENCODE_MODE) lzw_encode(dst, src);
    else lzw_decode(dst, src);

    fclose(dst);
    fclose(src);

    return EXIT_SUCCESS;
}
