//
// Created by freddy on 7/24/18.
//

#ifndef PROYECTO1_LIST_H
#define PROYECTO1_LIST_H

typedef struct node {
    char* flag;
    char* val;
    struct node *next;
} node;

typedef struct list {
    char* command;
    struct node *first;
    struct node *last;
} list;

list * newList();
void addNode(list *list, char *flag);

#endif //PROYECTO1_LIST_H
