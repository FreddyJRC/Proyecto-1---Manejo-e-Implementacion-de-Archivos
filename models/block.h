//
// Created by freddy on 8/25/18.
//

#ifndef PROYECTO1_SUPERBLOCK_H
#define PROYECTO1_SUPERBLOCK_H

#include <bits/types/time_t.h>

typedef struct SuperBlock{
    int s_filesystem_type;

    int s_inodes_count;
    int s_blocks_count;

    int s_free_blocks_count;
    int s_free_inodes_count;

    time_t s_mtime;
    time_t s_umtime;

    int s_mnt_count;

    int s_magic;

    int s_inode_size;
    int s_block_size;

    int s_first_ino;
    int s_first_blo;

    int s_bm_inode_start;
    int s_bm_block_start;

    int s_inode_start;
    int s_block_start;
} SuperBlock;

typedef struct content{
    char b_name[12];
    int b_inodo;
} content;

typedef struct dirBlock{
    content b_content[4];
} dirBlock;

typedef struct fileBlock{
    char b_content[64];
} fileBlock;

typedef struct pinterBlock{
    int b_pointers[16];
} pointerBlock;

#endif //PROYECTO1_SUPERBLOCK_H
