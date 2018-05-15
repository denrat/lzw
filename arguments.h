#ifndef _ARGUMENTS_H
#define _ARGUMENTS_H

#include <stdbool.h>

#define DECODE_MODE 0b0001
#define ENCODE_MODE 0b0010
#define STDOUT_MODE 0b0100

struct args
{
    int modes;
    char *input;
    char *output;
};

struct args parse_args(int, char *[]);
void print_help(char *, int);

#endif /* _ARGUMENTS_H */
