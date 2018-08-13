//
// Created by freddy on 7/24/18.
//

#include <stdlib.h>
#include <string.h>
#include "list.h"

list * newList(){
    list * new = malloc(sizeof(list));
    new->first = NULL;
    new->last = NULL;
    return new;
}

node * addNode(list *list, char *flag) {
    node *tmp = malloc(sizeof(node));
    tmp->next = NULL;
    strcpy(tmp->flag, flag);

    if(list->first == NULL){
        list->first = tmp;
        list->last = tmp;
    } else {
        list->last->next = tmp;
        list->last = tmp;
    }

    return tmp;
}

node * find(list *list, char *toFind){
    node *actual = list->first;

    while (actual != NULL){
        if(strcmp(actual->flag, toFind) == 0) { return actual; }
        actual = actual->next;
    }

    return NULL;
}
