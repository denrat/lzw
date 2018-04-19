#include <stdlib.h>
#include <stdio.h>

#include "stack.h"

void
push(int c, stack *st)
{
    node *n = (node *)malloc(sizeof(node));
    n->value = c;
    n->next = *st;

    *st = n;
}

void
print_stack(stack st)
{
    stack tmp = st;

    printf("[");
    while (tmp)
    {
        printf("%d", tmp->value);
        tmp = tmp->next;
        if (tmp) printf(", ");
    }
    printf("]\n");
}

int
pop(stack *st)
{
    int c = -1;

    if (!is_empty(*st))
    {
        c = head(*st);
        stack *oldn = st;
        *st = tail(*st);
        free_node(*oldn);
        printf("taking out %d, now stack is ", c);
        print_stack(*st);
    }

    return c;
}

int
head(stack st)
{
    return st->value;
}

stack
tail(stack st)
{
    return st->next;
}

int
is_empty(stack st)
{
    return st == NULL;
}

void
free_node(stack st)
{
    if (st)
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
