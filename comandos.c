//
// Created by freddy on 7/31/18.
//

#include <libgen.h>
#include <sys/stat.h>
#include "comandos.h"
bool mkdisk(list* list){
    node *size = NULL;
    node *path = NULL;
    node *unit = NULL;
    node *actual = list->first;

    while (actual != NULL){
        if(strcmp(actual->flag, "size") == 0) { size = actual; }
        if(strcmp(actual->flag, "path") == 0) { path = actual; }
        if(strcmp(actual->flag, "unit") == 0) { unit = actual; }
        actual = actual->next;
    }
    free(actual);

    if(size == NULL || path == NULL) {
        printf("ERROR: Parametros obligatorios no han sido especificados.\n");
        return true;
    }

    int s = atoi(size->val);

    if(s <= 0){
        printf("ERROR: Dimensiones invalidas.\n");
        return true;
    }

    char* t1 = strdup(path->val);
    char* dir = dirname(t1);
    if(stat(dir, NULL) == -1){
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "mkdir -p \"%s\"", dir);
        system(buffer);
    }
    free(t1);

    FILE *fp = fopen(path->val, "wb");
    if(fp != NULL){
        int z = 0;
        if(unit == NULL || strcmp(unit->val, "m") == 0 || strcmp(unit->val, "M") == 0){
            for (int i = 0; i <= s*1024*1024; ++i) {
                putc(0, fp);
            }
        }else if(strcmp(unit->val, "k") == 0 || strcmp(unit->val, "K") == 0){
            for (int i = 0; i <= s * 1024; ++i) {
                putc(0, fp);
            }
        }else{
            printf("ERROR: Unidad no valida.\n");
            fclose(fp);
            return true;
        }
    } else {
        printf("ERROR: Archivo no pudo ser creado.\n");
    }

    fclose(fp);
    printf("Disco creado exitosamente.\n");

    return true;
}