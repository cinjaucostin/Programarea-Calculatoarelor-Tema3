#ifndef utils
#define utils

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cJSON.h"

typedef struct
{
    unsigned char B;
    unsigned char G;
    unsigned char R;
} pixel;

char *change_name_task1(char *inputFileName);
char *change_name_task2(char *inputFileName);
int verify_line(int **number_matrix);
int verify_column(int **number_matrix);
int verify_square(int **number_matrix);
void create_json(int ok, char *inputFileName);
char *change_name_task3(char *inputFileName);
char *input_name(char *inputFileName);

#endif