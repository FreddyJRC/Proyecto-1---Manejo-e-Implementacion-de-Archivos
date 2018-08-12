//
// Created by freddy on 8/11/18.
//

#ifndef PROYECTO1_DISCOS_H
#define PROYECTO1_DISCOS_H

typedef struct particion {
    int id;
    char nombre[17];
    struct particion *next;
} particion;

typedef struct disco {
    char leter;
    struct disco *next;
    struct particion *particiones;
    char *path;
} disco;

#endif //PROYECTO1_DISCOS_H
