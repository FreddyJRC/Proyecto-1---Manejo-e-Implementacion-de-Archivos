//
// Created by freddy on 8/25/18.
//

#ifndef PROYECTO1_INODO_H
#define PROYECTO1_INODO_H

#include <bits/types/time_t.h>

typedef struct inodo{
    int i_uid;
    int i_gid;
    int i_size;
    time_t i_atime;
    time_t i_ctime;
    time_t i_mtime;
    int i_block[15];
    char i_type;
    int i_perm;
};

#endif //PROYECTO1_INODO_H
