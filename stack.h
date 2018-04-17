#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

typedef struct node {
    int value;
    struct node *next;
} node, *list;

void push(int, list *);
int pop(list *);
int head(list);

int is_empty(list);

void free_node(list);
void free_list(list *);

#endif /* _LINKED_LIST_H */
