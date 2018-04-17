#include <stdlib.h>
#include "stack.h"

void
push(int c, list *l)
{
    struct node *n = (struct node *)malloc(sizeof(struct node));
    n->value = c;
    n->next = *l;

    *l = n;
}

int
pop(list *l)
{
    int c = -1;

    if (!is_empty(*l))
    {
        c = (*l)->value;
        list *oldn = l;
        *l = (*l)->next;
        free_node(*oldn);
    }

    return c;
}

int
head(list l)
{
    return l->value;
}

int
is_empty(list l)
{
    return l == NULL;
}

void
free_node(list l)
{
    if (!is_empty(l))
    {
        free(l);
    }
}

void
free_list(list *l)
{
    list next;

    while (!is_empty(*l))
    {
        next = (*l)->next;
        free_node(*l);
        *l = next;
    }

    *l = NULL;
}
