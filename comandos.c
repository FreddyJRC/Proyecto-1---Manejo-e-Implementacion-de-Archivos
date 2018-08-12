//
// Created by freddy on 7/31/18.
//

#include <libgen.h>
#include <sys/stat.h>
#include <time.h>
#include "comandos.h"
#include "models/MBR.h"
#include "models/EBR.h"

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
            s = s * 1024 * 1024;
            for (int i = 0; i < s; ++i) {
                putc(0, fp);
            }
        }else if(strcmp(unit->val, "k") == 0 || strcmp(unit->val, "K") == 0){
            s = s * 1024;
            for (int i = 0; i < s; ++i) {
                putc(0, fp);
            }
        }else{
            printf("ERROR: unidad especificada no es valida. utilizado predefinida.\n");
            s = s * 1024 * 1024;
            for (int i = 0; i < s; ++i) {
                putc(0, fp);
            }
        }

        time_t timer;
        time(&timer);
        struct tm* tm_info;
        tm_info = localtime(&timer);

        printf("Disco creado exitosamente.\n");

        MBR *tabla = malloc(sizeof(MBR));
        tabla->mbr_size = s;
        tabla->mbr_disk_signature = (int) timer;
        strftime(tabla->mbr_creation_date, 17, "%d/%m/%Y %H:%M", tm_info);
        for (int j = 0; j < 4; ++j) {
            tabla->parts[j].part_status = 'f';
        }

        fseek(fp, 0, SEEK_SET);
        fwrite(tabla, sizeof(MBR), 1, fp);

        printf("MBR creado.\n");
        free(tabla);
        fclose(fp);
    } else {
        printf("ERROR: Archivo no pudo ser creado.\n");
        return true;
    }

    return true;
}

bool rmdisk(list* list){
    node *path = NULL;
    node *actual = list->first;

    while (actual != NULL){
        if(strcmp(actual->flag, "path") == 0) { path = actual; }
        actual = actual->next;
    }
    free(actual);

    if(path == NULL){
        printf("ERROR: Parametros obligatorios no especificados.\n");
        return true;
    }

    if(remove(path->val) == 0)
        printf("Disco eliminado exitosamente.\n");
    else
        printf("ERROR: No se ha podido eliminar el disco especificado.\n");

    return true;
}

bool fdisk(list* list){
    node *path = NULL, *size = NULL, *unit = NULL, *type = NULL;
    node *fit = NULL, *delete = NULL, *name = NULL, *add = NULL;
    node *actual = list->first;

    while (actual != NULL){
        if(strcmp(actual->flag, "path") == 0) { path = actual; }
        if(strcmp(actual->flag, "size") == 0) { size = actual; }
        if(strcmp(actual->flag, "unit") == 0) { unit = actual; }
        if(strcmp(actual->flag, "type") == 0) { type = actual; }
        if(strcmp(actual->flag, "fit") == 0) { fit = actual; }
        if(strcmp(actual->flag, "delete") == 0) { delete = actual; }
        if(strcmp(actual->flag, "name") == 0) { name = actual; }
        if(strcmp(actual->flag, "add") == 0) { add = actual; }
        actual = actual->next;
    }
    free(actual);

    if(path == NULL || name == NULL) {
        printf("ERROR: parametros obligatorios no especificados.\n");
        return true;
    }

    if(strlen(name->val) > 16){
        printf("ERROR: nombre debe ser menor a 16 caracteres./n");
        return true;
    }

    FILE *fp;

    if ((fp = fopen(path->val,"rb")) == NULL){
        printf("ERROR: no fue posible arrir el archivo.\n");
        return true;
    }

    MBR *tabla = malloc(sizeof(MBR));
    fread(tabla, sizeof(MBR), 1, fp);
    fclose(fp);

    if(size != NULL) {
        int part_available = 1;

        for (int i = 0; i < 4; ++i) {
            part_available = part_available * ((tabla->parts[i].part_status == 'f') ? 0 : 1);
        }

        if(part_available){
            printf("ERROR: Limite de particiones posibles excedido.\n");
            free(tabla);
            return true;
        }

        int s = atoi(size->val);

        if (s <= 0) {
            printf("ERROR: dimensiones ingresadas son invalidas.\n");
            free(tabla);
            return true;
        } else {
            if (unit == NULL) s = s * 1024;
            else {
                char u = tolower(unit->val[0]);
                if (u == 'b');
                else if (u == 'k') s = s * 1024;
                else if (u == 'm') s = s * 1024 * 1024;
                else {
                    printf("ERROR: unidad especificada no es valida.\n");
                    free(tabla);
                    return true;
                }
            }
        }

        if (s > (tabla->mbr_size - sizeof(MBR))) {
            printf("ERROR: espacio insuficiente.\n");
            free(tabla);
            return true;
        }

        char t = (type == NULL) ? 'p' : tolower(type->val[0]);

        if (t != 'p' && t != 'e' && t != 'l'){
            printf("ERROR: tipo de particion no es valido.\n");
            free(tabla);
            return true;
        }

        char f;

        if (fit != NULL){
            for ( ; *fit->val; ++fit->val) *fit->val = tolower(*fit->val);

            f = (strcmp(fit->val, "bf") == 0) ? 'b' :
                 (strcmp(fit->val, "ff") == 0) ? 'f' :
                 (strcmp(fit->val, "wf") == 0) ? 'w' : '\0';
        }else{
            f = 'w';
        }


        if (f == '\0') {
            printf("ERROR: tipo de ajuste no valido.\n");
            free(tabla);
            return true;
        }

        if (t == 'p' || t == 'e') {


            if (t == 'e') {
                for (int i = 0; i < 4; ++i) {
                    if (tabla->parts[i].part_status == 'a' && tabla->parts[i].part_type == 'e') {
                        printf("ERROR: Particion extendida ya presente.\n");
                        free(tabla);
                        return true;
                    }
                }
            }

            int st = sizeof(MBR);
            for (int i = 0; i < 4; ++i) {
                if (i == 3) {
                    if (tabla->parts[i].part_status == 'f' && tabla->mbr_size - st >= s) {
                        tabla->parts[i] = newPart('a', t, f, st+1, s, name->val);
                        break;
                    } else {
                        printf("ERROR: espacio insuficiente.\n");
                        free(tabla);
                        return true;
                    }
                }

                if (/*i == 0 &&*/ tabla->parts[i].part_status == 'a' && s <= tabla->parts[i].part_start - st) {
                //if (tabla->parts[i].part_status == 'f' && tabla->parts[i + 1].part_status == 'a'){
                    for (int j = 3; j > i; --j) {
                        if (tabla->parts[j].part_status == 'f' && tabla->parts[j-1].part_status == 'a'){
                            tabla->parts[j] = tabla->parts[j-1];
                            tabla->parts[j-1].part_status = 'f';
                        }
                    }

                    tabla->parts[i] = newPart('a', t, f, st + 1, s, name->val);
                    break;
                } else if (tabla->parts[i].part_status == 'f' && tabla->parts[i + 1].part_status == 'f') {
                    tabla->parts[i] = newPart('a', t, f, st + 1, s, name->val);
                    break;
                } else if (tabla->parts[i].part_status == 'a' && tabla->parts[i+1].part_status == 'a'){
                    if ((tabla->parts[i+1].part_start - (tabla->parts[i].part_start + tabla->parts[i].part_size)) >= s){
                        for (int j = 3; j > i+1; --j) {
                            if (tabla->parts[j].part_status == 'f' && tabla->parts[j-1].part_status == 'a'){
                                tabla->parts[j] = tabla->parts[j-1];
                                tabla->parts[j-1].part_status = 'f';
                            }
                        }

                        tabla->parts[i+1] = newPart('a', t, f, st + 1, s, name->val);
                        break;
                    }
                }

                st = (tabla->parts[i].part_start - 1) + tabla->parts[i].part_size;
            }

            fp = fopen(path->val, "rb+");
            if(fp != NULL){
                fseek(fp, 0, SEEK_SET);
                fwrite(tabla, sizeof(MBR), 1, fp);
                if(t == 'e'){
                    EBR *newExtended = malloc(sizeof(EBR));
                    newExtended->part_start = st+1;
                    newExtended->part_status = 'f';
                    newExtended->part_next = -1;
                    newExtended->part_size = 0;
                    fseek(fp, st+1, SEEK_SET);
                    fwrite(newExtended, sizeof(EBR), 1, fp);
                    free(newExtended);
                }
                fclose(fp);
                printf("Particion creada exitosamente.\n");
            }
        } else {
            /*CREACION DE PARTICIONES LOGICAS*/
            part extended_part;
            extended_part.part_status = -1;

            for (int i = 0; i < 4; ++i) {
                if (tabla->parts[i].part_status == 'a' && tabla->parts[i].part_type == 'e') {
                    extended_part = tabla->parts[i];
                    break;
                }
            }

            if (extended_part.part_status == -1) {
                printf("Particion extendida no encontrada.\n");
                free(tabla);
                return true;
            }

            if (s > (extended_part.part_size - sizeof(EBR))) {
                printf("Espacio insuficiente.\n");
                free(tabla);
                return true;
            }

            EBR *ebr_list;

            if ((fp = fopen(path->val, "rb")) == NULL) {
                printf("ERROR: no fue posible arrir el archivo.\n");
                return true;
            } else {
                ebr_list = malloc(sizeof(EBR));

                fseek(fp, extended_part.part_start, SEEK_SET);
                fread(ebr_list, sizeof(EBR), 1, fp);
                fclose(fp);
            }

            if (ebr_list->part_status == 'f') {
                if (ebr_list->part_next == -1){
                    if ((s) <= (extended_part.part_size - (extended_part.part_start - 1))){
                        ebr_list->part_start = ebr_list->part_start;
                        ebr_list->part_size = s;
                        ebr_list->part_next = -1;
                        ebr_list->part_status = 'a';
                        strcpy(ebr_list->part_name, name->val);
                        ebr_list->part_fit = f;

                        if((fp = fopen(path->val, "rb+")) != NULL) {
                            fseek(fp, ebr_list->part_start, SEEK_SET);
                            fwrite(ebr_list, sizeof(EBR), 1, fp);

                            fclose(fp);
                            free(ebr_list);
                            free(tabla);
                            printf("Particion logica creada exitosamente.\n");
                            return true;
                        }

                    } else {
                        printf("Espacio insuficiente.\n");
                        free(ebr_list);
                        free(tabla);
                        return true;
                    }

                } else {
                    if ((s) <= (ebr_list->part_next - (ebr_list->part_start - 1))) {
                        ebr_list->part_start = ebr_list->part_start;
                        ebr_list->part_size = s;
                        ebr_list->part_next = ebr_list->part_next;
                        ebr_list->part_status = 'a';
                        strcpy(ebr_list->part_name, name->val);
                        ebr_list->part_fit = f;

                        if((fp = fopen(path->val, "rb+")) != NULL) {
                            fseek(fp, ebr_list->part_start, SEEK_SET);
                            fwrite(ebr_list, sizeof(EBR), 1, fp);

                            printf("Particion logica creada exitosamente.\n");
                            fclose(fp);
                            free(ebr_list);
                            free(tabla);
                            return true;
                        }
                    }
                }
            }


            int st = extended_part.part_start;
            do {
                if((fp = fopen(path->val, "rb+")) != NULL) {
                    fseek(fp, st, SEEK_SET);
                    fread(ebr_list, sizeof(EBR), 1, fp);

                    if (ebr_list->part_next == -1){
                        if ((s) <= (((extended_part.part_start - 1) + extended_part.part_size)
                                        - ((ebr_list->part_start - 1) + ebr_list->part_size))){

                            ebr_list->part_next = ebr_list->part_start + ebr_list->part_size;

                            fseek(fp, ebr_list->part_start, SEEK_SET);
                            fwrite(ebr_list, sizeof(EBR), 1, fp);

                            EBR *newEBR = malloc(sizeof(EBR));
                            newEBR->part_start = ebr_list->part_next;
                            newEBR->part_next = -1;
                            newEBR->part_size = s;
                            newEBR->part_fit = f;
                            newEBR->part_status = 'a';
                            strcpy(newEBR->part_name, name->val);

                            fseek(fp, newEBR->part_start, SEEK_SET);
                            fwrite(newEBR, sizeof(EBR), 1, fp);
                            fclose(fp);
                            free(newEBR);

                            printf("Particion logica creada exitosamente.\n");
                            break;

                        } else {
                            printf("Espacio insuficiente.\n");
                            break;
                        }
                    }else{
                        if ((s) <= (ebr_list->part_next - ((ebr_list->part_start - 1) + ebr_list->part_size))) {

                            EBR *newEBR = malloc(sizeof(EBR));
                            newEBR->part_start = (ebr_list->part_start) + ebr_list->part_size;
                            newEBR->part_next = ebr_list->part_next;
                            newEBR->part_size = s;
                            newEBR->part_fit = f;
                            newEBR->part_status = 'a';
                            strcpy(newEBR->part_name, name->val);

                            fseek(fp, newEBR->part_start, SEEK_SET);
                            fwrite(newEBR, sizeof(EBR), 1, fp);

                            ebr_list->part_next = newEBR->part_start;

                            fseek(fp, ebr_list->part_start, SEEK_SET);
                            fwrite(ebr_list, sizeof(EBR), 1, fp);
                            fclose(fp);
                            free(newEBR);

                            printf("Particion logica creada exitosamente.\n");
                            break;

                        }
                    }
                    fclose(fp);
                    st = ebr_list->part_next;
                }

            } while (st != -1);


            free(ebr_list);
            free(tabla);
            return true;
        }

    } else if (add != NULL){

        int s = atoi(add->val); //Valor a agumentar/disminuir la particion.

        if (unit == NULL) s = s * 1024;
        else {
            char u = tolower(unit->val[0]);
            if (u == 'b');
            else if (u == 'k') s = s * 1024;
            else if (u == 'm') s = s * 1024 * 1024;
            else {
                printf("ERROR: unidad especificada no es valida.\n");
                free(tabla);
                return true;
            }
        }

        for (int i = 0; i < 4; ++i) {
            if (tabla->parts[i].part_status == 'a' && strcmp(tabla->parts[i].part_name, name->val) == 0){

                if (i == 3) {
                    if (((tabla->parts[i].part_start - 1) + tabla->parts[i].part_size + s) > tabla->parts[i].part_start &&
                            ((tabla->parts[i].part_start - 1) + tabla->parts[i].part_size + s) <= tabla->mbr_size) {

                        tabla->parts[i].part_size = tabla->parts[i].part_size + s;

                        fp = fopen(path->val, "rb+");
                        if(fp != NULL){
                            fseek(fp, 0, SEEK_SET);
                            fwrite(tabla, sizeof(MBR), 1, fp);

                            fclose(fp);
                            printf("Particion modificada exitosamente.\n");
                        }
                        free(tabla);
                        return true;

                    } else {
                        printf("ERROR: espacio insuficiente.\n");
                        free(tabla);
                        return true;
                    }
                }


                if (((tabla->parts[i].part_start - 1) + tabla->parts[i].part_size + s) > tabla->parts[i].part_start &&
                    ((tabla->parts[i].part_start - 1) + tabla->parts[i].part_size + s)
                        <= ((tabla->parts[i+1].part_status == 'a') ? tabla->parts[i+1].part_start : tabla->mbr_size)) {

                    tabla->parts[i].part_size = tabla->parts[i].part_size + s;

                    fp = fopen(path->val, "rb+");
                    if(fp != NULL){
                        fseek(fp, 0, SEEK_SET);
                        fwrite(tabla, sizeof(MBR), 1, fp);

                        fclose(fp);
                        printf("Particion modificada exitosamente.\n");
                    }
                    free(tabla);
                    return true;

                }
            }
        }

        EBR *ebr_list = malloc(sizeof(EBR));

        for (int i = 0; i < 4; ++i) {
            if (tabla->parts[i].part_status == 'a' && tabla->parts[i].part_type == 'e'){
                int st = tabla->parts[i].part_start;
                do {
                    if((fp = fopen(path->val, "rb+")) != NULL) {
                        fseek(fp, st, SEEK_SET);
                        fread(ebr_list, sizeof(EBR), 1, fp);

                        if (ebr_list->part_status == 'a' && strcmp(ebr_list->part_name, name->val) == 0){
                            if (((ebr_list->part_start - 1) + ebr_list->part_size + s) > ebr_list->part_start &&
                                    (ebr_list->part_start - 1) + ebr_list->part_size + s
                                            <= ((ebr_list->part_next != -1) ? (ebr_list->part_next - 1)
                                                    : ((tabla->parts[i].part_start - 1) + tabla->parts[i].part_size))) {

                                ebr_list->part_size = ebr_list->part_size + s;

                                fseek(fp, ebr_list->part_start, SEEK_SET);
                                fwrite(ebr_list, sizeof(EBR), 1, fp);
                                fclose(fp);
                                free(ebr_list);
                                free(tabla);

                                printf("Particion logica modificada exitosamente.\n");
                                return true;
                            }
                        }

                        fclose(fp);
                        st = ebr_list->part_next;
                    }

                } while (st != -1);
            }
        }

    } else if (delete != NULL){
        printf("Seguro que desea eliminar la particion %s [Y/n]:", name->val);
        char v = getc(stdin);
        printf("\n");

        if (tolower(v) == 'y') {
            for (; *delete->val; ++delete->val) *delete->val = tolower(*delete->val);

            for (int i = 0; i < 4; ++i) {
                if (tabla->parts[i].part_status == 'a' && strcmp(tabla->parts[i].part_name, name->val) == 0) {

                    if (strcmp(delete->val, "fast") == 0){
                        tabla->parts[i].part_size = 0;
                        tabla->parts[i].part_start = 0;
                        tabla->parts[i].part_status = 'f';

                        fp = fopen(path->val, "rb+");
                        if(fp != NULL){
                            fseek(fp, 0, SEEK_SET);
                            fwrite(tabla, sizeof(MBR), 1, fp);

                            fclose(fp);
                            printf("Particion eliminada exitosamente.\n");
                        }
                        free(tabla);
                        return true;
                    } else if (strcmp(delete->val, "full") == 0) {
                        fp = fopen(path->val, "rb+");
                        if (fp != NULL){
                            fseek(fp, tabla->parts[i].part_start, SEEK_SET);
                            for (int j = 0; j < tabla->parts[i].part_size; j++) {
                                fputc(0, fp);
                            }

                            tabla->parts[i].part_size = 0;
                            tabla->parts[i].part_start = 0;
                            tabla->parts[i].part_status = 'f';

                            fseek(fp, 0, SEEK_SET);
                            fwrite(tabla, sizeof(MBR), 1, fp);

                            fclose(fp);
                            printf("Particion eliminada exitosamente.\n");
                        }
                        free(tabla);
                        return true;
                    }

                }
            }

            EBR *ebr_list = malloc(sizeof(EBR));
            ebr_list->part_status = 0;

            EBR *ebr_last;

            for (int i = 0; i < 4; ++i) {
                if (tabla->parts[i].part_status == 'a' && tabla->parts[i].part_type == 'e') {
                    int st = tabla->parts[i].part_start;
                    do {
                        if ((fp = fopen(path->val, "rb+")) != NULL) {
                            ebr_last = ebr_list;
                            fseek(fp, st, SEEK_SET);
                            fread(ebr_list, sizeof(EBR), 1, fp);

                            if (ebr_list->part_status == 'a' && strcmp(ebr_list->part_name, name->val) == 0) {
                                if (strcmp(delete->val, "fast") == 0){

                                    if (ebr_last->part_status == 0){
                                        ebr_list->part_status = 'f';
                                        ebr_list->part_size = 0;

                                        fseek(fp, ebr_list->part_start, SEEK_SET);
                                        fwrite(ebr_list, sizeof(EBR), 1, fp);
                                        fclose(fp);
                                        printf("Particion eliminada exitosamente.\n");
                                    } else {
                                        ebr_last->part_next = ebr_list->part_next;

                                        fseek(fp, ebr_last->part_start, SEEK_SET);
                                        fwrite(ebr_last, sizeof(EBR), 1, fp);
                                        fclose(fp);
                                        printf("Particion eliminada exitosamente.\n");
                                    }
                                    free(tabla);
                                    return true;

                                } else if (strcmp(delete->val, "full")){
                                    if (ebr_last->part_status == 0){
                                        ebr_list->part_status = 'f';
                                        ebr_list->part_size = 0;

                                        fseek(fp, ebr_list->part_start + sizeof(EBR), SEEK_SET);
                                        for (int j = 0; j < ebr_list->part_size - sizeof(EBR); ++j) {
                                            fputc(0, fp);
                                        }

                                        fseek(fp, ebr_list->part_start, SEEK_SET);
                                        fwrite(ebr_list, sizeof(EBR), 1, fp);
                                        fclose(fp);
                                        printf("Particion eliminada exitosamente.\n");
                                    } else {
                                        ebr_last->part_next = ebr_list->part_next;

                                        fseek(fp, ebr_list->part_start, SEEK_SET);
                                        for (int j = 0; j < ebr_list->part_size; ++j) {
                                            fputc(0, fp);
                                        }

                                        fseek(fp, ebr_last->part_start, SEEK_SET);
                                        fwrite(ebr_last, sizeof(EBR), 1, fp);
                                        fclose(fp);
                                        printf("Particion eliminada exitosamente.\n");
                                    }
                                    free(tabla);
                                    return true;
                                }
                            }

                            fclose(fp);
                            st = ebr_list->part_next;
                        }

                    } while (st != -1);
                }
            }
        }
    }

    free(tabla);
    return true;
}

bool mount(list* list, disco **dsc){
    node *path = NULL, *name = NULL;
    node *actual = list->first;

    while (actual != NULL){
        if(strcmp(actual->flag, "path") == 0) { path = actual; }
        if(strcmp(actual->flag, "name") == 0) { name = actual; }
        actual = actual->next;
    }
    free(actual);

    if (path == NULL || name == NULL){
        printf("ERROR: Parametros obligatorios no encontrados.\n");
        return true;
    }

    //-> VALIDAR QUE EXISTA.
    FILE *fp;

    if ((fp = fopen(path->val,"rb")) == NULL){
        printf("ERROR: no fue posible arrir el archivo.\n");
        return true;
    }

    MBR *tabla = malloc(sizeof(MBR));
    fread(tabla, sizeof(MBR), 1, fp);
    fclose(fp);

    bool found = false;
    int e = -1;
    for (int i = 0; i < 4; ++i) {
        if (strcmp(tabla->parts[i].part_name, name->val) == 0 && tabla->parts[i].part_status == 'a'){
            found = true;
            break;
        } else if (tabla->parts[i].part_status == 'a' && tabla->parts[i].part_type == 'e'){
            e = i;
            break;
        }
    }

    if (!found){
        if(e == -1) {
            printf("ERROR: Particion no encontrada.\n");
            free(tabla);
            return true;
        } else {
            EBR *ebr_list = malloc(sizeof(EBR));

            int st = tabla->parts[e].part_start;
            do {
                if ((fp = fopen(path->val, "rb+")) != NULL) {
                    fseek(fp, st, SEEK_SET);
                    fread(ebr_list, sizeof(EBR), 1, fp);

                    if (ebr_list->part_status == 'a' && strcmp(ebr_list->part_name, name->val) == 0) {
                        found = true;
                        break;
                    }

                }
                fclose(fp);
                st = ebr_list->part_next;
            }while (st != -1);

            if (!found){
                printf("ERROR: Particion no encontrada.\n");
                free(tabla);
                return true;
            }
        }
    }

    disco *discos = *dsc;

    if(discos->leter == '\0'){

        particion *nueva = malloc(sizeof(particion));
        nueva->id = 1;
        strcpy(nueva->nombre, name->val);
        nueva->next = NULL;

        discos = malloc(sizeof(disco));

        (discos)->leter = 'a';
        discos->path = malloc(sizeof(path->val));
        strcpy(discos->path, path->val);
        (discos)->particiones = nueva;
        (discos)->next = NULL;

        printf("Mounted en: vd%c%d.\n", discos->leter, nueva->id);
        *dsc = discos;
    } else {
        disco *pDisco = (discos);
        while (pDisco != NULL){
            if(strcmp(pDisco->path, path->val) == 0){
                particion *pPart = pDisco->particiones;
                if (pPart != NULL) {
                    while (pPart->next != NULL) { pPart = pPart->next; }

                    particion *nueva = malloc(sizeof(particion));
                    nueva->next = NULL;
                    nueva->id = pPart->id + 1;
                    strcpy(nueva->nombre, name->val);
                    pPart->next = nueva;

                    printf("Mounted en: vd%c%d.\n", pDisco->leter, nueva->id);
                } else {
                    particion *nueva = malloc(sizeof(particion));
                    nueva->next = NULL;
                    nueva->id = 1;
                    strcpy(nueva->nombre, name->val);
                    pDisco->particiones = nueva;

                    printf("Mounted en: vd%c%d.\n", pDisco->leter, nueva->id);
                }

                *dsc = discos;

                return true;
            }

            pDisco = pDisco->next;
        }

        pDisco = discos;
        while (pDisco->next != NULL) { pDisco = pDisco->next; }

        particion *nueva = malloc(sizeof(particion));
        nueva->next = NULL;
        nueva->id = 1;
        strcpy(nueva->nombre, name->val);

        disco *nuevo = malloc(sizeof(disco) + sizeof(char [strlen(path->val)]));
        nuevo->leter = (char)(((int)pDisco->leter) + 1);
        nuevo->path = malloc(sizeof(path->val));
        strcpy(nuevo->path, path->val);
        nuevo->particiones = nueva;
        nuevo->next = NULL;
        pDisco->next = nuevo;

        printf("Mounted en: vd%c%d.\n", pDisco->leter, nueva->id);
        *dsc = discos;
    }

    return true;
}

bool unmount(list* list, disco **discos){
    node *id = NULL;
    node *actual = list->first;

    while (actual != NULL){
        if(strcmp(actual->flag, "id") == 0) { id = actual; }
        actual = actual->next;
    }
    free(actual);

    if (id == NULL){
        printf("ERROR: parametros requeridos no encontrados.");
        return true;
    }

    char d = id->val[2];
    int p = atoi(&id->val[3]);

    disco *pDisco = *discos;

    while (pDisco != NULL){

        if (pDisco->leter == d){
            particion *currP, *prevP;

            for (currP = pDisco->particiones; currP != NULL; prevP = currP, currP = currP->next) {

                if (currP->id == p) {
                    if (prevP == NULL) {
                        pDisco->particiones = currP->next;
                    } else {
                        prevP->next = currP->next;
                    }

                    free(currP);
                    printf("Particion desmontada.\n");
                    return true;
                }
            }
        }

        pDisco = pDisco->next;
    }

    return true;
}

bool rep(list* list, disco **discos){
    node *path = NULL, *name = NULL, *add = NULL;
    node *actual = list->first;

    while (actual != NULL){
        if(strcmp(actual->flag, "path") == 0) { path = actual; }
        if(strcmp(actual->flag, "name") == 0) { name = actual; }
        if(strcmp(actual->flag, "add") == 0) { add = actual; }
        actual = actual->next;
    }
    free(actual);

    return true;
}