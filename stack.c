#include <stdlib.h>
#include "stack.h"

void
push(int c, stack *st)
{
    struct node *n = (struct node *)malloc(sizeof(struct node));
    n->value = c;
    n->next = *st;

    *st = n;
}

int
pop(stack *st)
{
    int c = -1;

    if (!is_empty(*st))
    {
        c = (*st)->value;
        stack *oldn = st;
        *st = (*st)->next;
        free_node(*oldn);
    }

    return c;
}

int
head(stack st)
{
    return st->value;
}

int
is_empty(stack st)
{
    return st == NULL;
}

void
free_node(stack st)
{
    if (!is_empty(st))
    {
        free(st);
    }
}

void
free_stack(stack *st)
{
    stack next;

    while (!is_empty(*st))
    {
        next = (*st)->next;
        free_node(*st);
        *st = next;
    }

    *st = NULL;
}
