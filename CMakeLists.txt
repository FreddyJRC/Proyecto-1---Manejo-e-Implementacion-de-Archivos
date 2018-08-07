cmake_minimum_required(VERSION 3.12)
project(Proyecto1 C)

set(CMAKE_C_STANDARD 99)

add_executable(Proyecto1 main.c models/list.c models/list.h comandos.c comandos.h models/MBR.c models/MBR.h models/EBR.c models/EBR.h)