#ifndef _STACK_H
#define _STACK_H

typedef struct node {
    int value;
    struct node *next;
} node, *stack;

void push(int, stack *);
int pop(stack *);
int head(stack);

int is_empty(stack);

void free_node(stack);
void free_stack(stack *);

#endif /* _STACK_H */
