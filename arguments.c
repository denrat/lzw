#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "arguments.h"

struct args
parse_args(int argc, char *argv[])
{
    struct args args;
    args.modes = 0;

    // Check whether to print help
    if (argc == 1) print_help(EXIT_FAILURE);

    if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
    {
        print_help(EXIT_SUCCESS);
    }

    if (!strcmp(argv[1], "e") || !strcmp(argv[1], "encode")
            || !strcmp(argv[1], "d") || !strcmp(argv[1], "decode"))
    {
        if (argc == 1) print_help(EXIT_FAILURE);

        args.modes |= (argv[1][0] == 'e') ? ENCODE_MODE : DECODE_MODE;
        args.input = argv[2];
    }

    if (argc < 4)
    {
        char *fmt = (args.modes & ENCODE_MODE) ? "%s.lzw" : "%s.unlzw";
        asprintf(&args.output, fmt, args.input);
    }
    else if (!strcmp(argv[3], "--to-stdout"))
    {
        if (argc > 4) print_help(EXIT_FAILURE);

        args.modes |= STDOUT_MODE;
    }
    else if (!strcmp(argv[3], "-o") || !strcmp(argv[3], "--output"))
    {
        if (argc < 5) print_help(EXIT_FAILURE);

        args.output = argv[4];
    }
    else
    {
        print_help(EXIT_FAILURE);
    }

    return args;
}

void
print_help(int exit_code)
{
    printf("help page here\n");
    exit(exit_code);
}
