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
    if (argc == 1) print_help(argv[0], EXIT_FAILURE);

    // Parse first argument
    if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
    {
        // Help asked
        print_help(argv[0], EXIT_SUCCESS);
    }
    else if (!strcmp(argv[1], "e") || !strcmp(argv[1], "encode")
            || !strcmp(argv[1], "d") || !strcmp(argv[1], "decode"))
    {
        // Too few args supplied
        if (argc == 2) print_help(argv[0], EXIT_FAILURE);

        // Determine action to carry out
        args.modes |= (argv[1][0] == 'e') ? ENCODE_MODE : DECODE_MODE;
        args.input = argv[2];
    }
    else
    {
        // Neither -h, --help, e[ncode] or d[ecode]
        print_help(argv[0], EXIT_FAILURE);
    }

    if (argc < 4)
    {
        // Define name of the file
        char *fmt = (args.modes & ENCODE_MODE) ? "%s.lzw" : "%s.unlzw";
        asprintf(&args.output, fmt, args.input);
    }
    else if (!strcmp(argv[3], "--to-stdout"))
    {
        // Too many args supplied
        if (argc > 4) print_help(argv[0], EXIT_FAILURE);

        args.modes |= STDOUT_MODE;
    }
    else if (!strcmp(argv[3], "-o") || !strcmp(argv[3], "--output"))
    {
        if (argc < 5) print_help(argv[0], EXIT_FAILURE);

        args.output = argv[4];
    }
    else
    {
        print_help(argv[0], EXIT_FAILURE);
    }

    return args;
}

void
clean_args(struct args *args)
{
    // asprintf requires freeing
    free(args->output);
}

void
print_help(char cliname[], int exit_code)
{
    printf("Usage:\n");
    printf("\tCompress/decompress file\n");
    printf("\t\t%s COMMAND FILE\n", cliname);
    printf("\tCompress/decompress file, specifying name in output\n");
    printf("\t\t%s COMMAND FILE (-o|--output) FILEOUT\n", cliname);
    printf("\tCompress/decompress file, sending the processed data to stdout\n");
    printf("\t\t%s COMMAND FILE --to-stdout\n", cliname);
    exit(exit_code);
}
