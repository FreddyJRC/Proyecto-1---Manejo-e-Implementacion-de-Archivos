//
// Created by freddy on 8/2/18.
//

#ifndef PROYECTO1_MBR_H
#define PROYECTO1_MBR_H

typedef struct part{
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_size;
    char part_name[17];
} part;

typedef struct MBR{
    int mbr_size;
    char mbr_creation_date[17];
    int mbr_disk_signature;
    part parts[4];
} MBR;

part newPart(char part_status, char part_type, char part_fit, int part_start, int part_size, char part_name[17]);

#endif //PROYECTO1_MBR_H
