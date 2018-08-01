#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "comandos.h"
#include "models/list.h"

bool menu(char *command);
bool abrir(list *pList);
char *scanner(FILE* fp, size_t size);

list *parse(char *command);

char * concat(char *cat, char i);

int main() {
    char *command;
    char *pos;
    bool loop = true;

    printf("Manejo e Implementacion de Archivos\n"
           "Proyecto 1\n"
           "Freddy Jose Ramirez Contreras\n"
           "201403780\n");

    while(loop){
        printf("\ndisk>");
        command = scanner(stdin, 20);

        if ((pos=strchr(command, '\n')) != NULL)
            *pos = '\0';

        loop = menu(command);
    }
    return 0;
}

bool menu(char *command) {
    if(strlen(command) <= 0) return true;
    if(strcmp(command, "salir") == 0) {
        return false;
    }

    bool res = true;

    list *flags = parse(command);
//    flag = strtok(command, " ");
//    while(flag != NULL) {
//        addNode(flags, flag);
//        flag = strtok(NULL, " ");
//    }
//    free(flag);

    if(strcmp(flags->command, "abrir") == 0){
        //res = abrir(flags);
    }

    printf("Comando: %s\n", flags->command);
    node *actual = flags->first;
    while (actual != NULL){
        printf("%s: %s\n", actual->flag, actual->val);
        actual = actual->next;
    }

    free(flags);
    return res;
}

bool abrir(list *flags) {
    char *path = flags->first->next->val;
    if(path == NULL) {
        printf("ERROR: La direccion del archivo no ha sido especificada.");
        return 1;
    }

    char * line = NULL;
    size_t len = 0;
    FILE *fp = fopen(path, "r");
    if(fp == NULL) {
        printf("ERROR: El archivo especificado no ha podido ser abierto.");
        return 1;
    }

    bool res;
    char *pos;
    while (getline(&line, &len, fp) != -1){
        if ((pos=strchr(line, '\n')) != NULL)
            *pos = '\0';

        res = menu(line);
        if (!res) break;
    }

    fclose(fp);
    return res;
}

char *scanner(FILE *fp, size_t size) {
    char *str;
    int ch;
    size_t len = 0;
    str = realloc(NULL, sizeof(char)*size);//size is start size
    if(!str)return str;
    while(EOF!=(ch=fgetc(fp)) && ch != '\n'){
        str[len++]=ch;
        if(len==size){
            str = realloc(str, sizeof(char)*(size+=16));
            if(!str)return str;
        }
    }
    str[len++]='\0';

    return realloc(str, sizeof(char)*len);
}

list *parse(char *input) {
    int status = 0, string = 0;
    list *res = newList();
    node *newNode;
    char *cat = "";
    char *c;
    c = input;

    do{
        if (status == 0){
            if(*c != ' '){
                cat = concat(cat, tolower(*c));
            }else{
                res->command = cat;
                cat = "";
                status = 1;
            }
        }else if(status == 1){
            if(*c == '-'){ }
            else if(*c == '>'){
                addNode(res, cat);

                cat = "";
                status = 2;
            }else if(*c != ' '){
                cat = concat(cat, tolower(*c));
            }
        }else if(status == 2){
            if(*c == '"') {
                string = (string) ? 0 : 1;
            }else if(*c == ' ' || *c == '\0'){
                if (string){
                    cat = concat(cat, *c);
                }else{
                    res->last->val = cat;

                    cat = "";
                    status = 1;
                    if (*c == '\0') { break; }
                }
            }else{
                cat = concat(cat, *c);
            }
        }

    }while(++c);

    return res;
}

char * concat(char *cat, char i) {
    size_t len = strlen(cat);
    char *res = malloc(len + 1 + 1);
    strcpy(res, cat);
    res[len] = i;
    res[len + 1] = '\0';

    return res;
}