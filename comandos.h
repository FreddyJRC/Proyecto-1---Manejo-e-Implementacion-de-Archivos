//
// Created by freddy on 7/31/18.
//

#ifndef PROYECTO1_COMANDOS_H
#define PROYECTO1_COMANDOS_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "models/list.h"
#include "models/discos.h"

bool mkdisk(list* list);
bool rmdisk(list* list);
bool fdisk(list* list);
bool mount(list* list, disco **discos);
bool unmount(list* list, disco **discos);
bool rep(list* list, disco **dsc);

#endif //PROYECTO1_COMANDOS_H
