#include <stdio.h>
#include <sys/stat.h>

#include "tools.h"
#include "triple.h"

void
emit_code(FILE *dst, FILE *src, int code)
{
    static triple t;
    static int codebuf[2];
    static int count = 0;

    // Store code to be written
    codebuf[count++] = code;

    if (count == 2)
    {
        // Buffer is full, encode values and write to dst
        triple_encode(t, codebuf[0], codebuf[1]);
        fwrite(t, sizeof(triple), 1, dst);

        count = 0;
    }
}
