//
// Created by freddy on 8/2/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "MBR.h"

part newPart(char part_status, char part_type, char part_fit, int part_start, int part_size, char part_name[17]){
    part new;

    new.part_status = part_status;
    new.part_type = part_type;
    new.part_fit = part_fit;
    new.part_start = part_start;
    new.part_size = part_size;
    strcpy(new.part_name, part_name);

    return new;
}
