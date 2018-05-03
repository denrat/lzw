#ifndef _TRIPLE_H
#define _TRIPLE_H

typedef unsigned char byte;
typedef unsigned char triple[3];

void triple_encode(triple, int, int);
void triple_decode(triple, int *, int *);

#endif /* _TRIPLE_H */
