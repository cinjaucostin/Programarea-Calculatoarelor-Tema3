#include "utils.h"

char *change_name_task1(char *inputFileName)
{
    char *outputFileName, *prefix, nr1, nr2;
    outputFileName = calloc(100, sizeof(char));
    prefix = strstr(inputFileName, "board");//selectam doar partea din string care urmeaza dupa cuvantul board(inclusiv acesta)
    nr1 = prefix[5];//pe pozitia 5 se afla prima cifra din numarul board-ului de input
    nr2 = prefix[6];//pe pozitia 6 se afla a doua cifra din numarul board-ului de input
    sprintf(outputFileName, "output_task1_board%c%c.bmp", nr1, nr2);//formatul specific pentru numele fisierului pentru task1
    return outputFileName;
}

char *change_name_task2(char *inputFileName) // aceeasi idee ca si la change_name_task1 doar ca numele este adaptat pentru task2
{
    char *outputFileName, *prefix, nr1, nr2;
    outputFileName = calloc(100, sizeof(char));
    prefix = strstr(inputFileName, "board");
    nr1 = prefix[5];
    nr2 = prefix[6];
    sprintf(outputFileName, "output_task2_board%c%c.bmp", nr1, nr2);
    return outputFileName;
}

int verify_line(int **number_matrix)//functia care verifica daca un numar apare doar o singura data(pentru a respecta  prima regula a jocului)
{
    int ok = 1;
    int *freq;
    freq = calloc(10, sizeof(int));
    for (int i = 0; i < 9 && ok == 1; i++)//am facut verificarea ok==1 pentru eficientizare
    {
        for (int j = 0; j < 9; j++)
        {
            freq[number_matrix[i][j]]++;//ne folosim de un vector de frecventa in care numaram aparitiile fiecarui element
        }
        for (int j = 1; j <= 9; j++)
        {
            if (freq[j] != 1)//in cazul in care aparitia este diferita de 1 inseamna ca numarul nu apare o singura data
            {
                ok = 0;
                break;
            }
        }
        for (int j = 1; j <= 9; j++)//resetam valorile din vectorul de frecventa pentru a verifica si urmatoarea linie
        {
            freq[j] = 0;
        }
    }
    free(freq);
    return ok;
}
int verify_column(int **number_matrix)//aceeasi idee ca la verify_line doar ca adaptata pentru coloane
{
    int ok = 1;
    int *freq;
    freq = calloc(10, sizeof(int));
    for (int i = 0; i < 9 && ok == 1; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            freq[number_matrix[j][i]]++;
        }
        for (int j = 1; j <= 9; j++)
        {
            if (freq[j] != 1)
            {
                ok = 0;
            }
        }
        for (int j = 1; j <= 9; j++)
        {
            freq[j] = 0;
        }
    }
    free(freq);
    return ok;
}

int verify_square(int **number_matrix)
{
    int ok = 1;
    int *freq;
    freq = calloc(10, sizeof(int));
    for (int i = 0; i < 3 && ok == 1; i++)//verificam patratele de suprafata 3x3(pentru a verifica cea de-a treuia regula)
    {
        for (int j = 0; j < 3 && ok == 1; j++)
        {
            for (int k = 3 * i; k <= 3 * i + 2; k++)//de ex. primul patratel se afla de la linia 0 pana la linia 2 si de la coloana 0 pana la coloana 2
            {
                for (int l = 3 * j; l <= 3 * j + 2; l++)
                {
                    freq[number_matrix[k][l]]++;
                }
            }
            for (int k = 1; k <= 9; k++)//in rest este aceeasi idee cu vectorul de frecventa
            {
                if (freq[k] != 1)
                {
                    ok = 0;
                }
            }
            for (int k = 1; k <= 9; k++)
            {
                freq[k] = 0;
            }
        }
    }
    free(freq);
    return ok;
}

void create_json(int ok,char *inputFileName)
{
    char *content = NULL,*outputFileName;
    cJSON *result = cJSON_CreateObject();//am creat un obiect de tip json
    cJSON *input_file = NULL, *message = NULL;
    input_file = cJSON_CreateString(input_name(inputFileName));
    cJSON_AddItemToObject(result, "input_file", input_file);
    if (ok == 1)//Cazul in care toate regulile jocului sunt respectate si introducem in message stringul specific
        message = cJSON_CreateString("Win!");
    else if (ok == 0)//Cazul in care una dintre reguli este incalcata si introducem in message stringul "Loss :("
        message = cJSON_CreateString("Loss :(");
    cJSON_AddItemToObject(result, "game_state", message);//introducem in obiectul result JSON 
    content = cJSON_Print(result);//"Printam" in pointerul content continutul obiectului de tip JSON
    outputFileName=change_name_task3(inputFileName);//am creat numele fisierului in care voi scrie continutul pointerului content
    FILE *json_output;
    json_output = fopen(outputFileName, "w");//deschidem fisierul in care scriem
    fprintf(json_output, "%s\n", content);//ne folosim de functia fprintf pentru a scrie in fisier continutul pointerului content
    free(content);//eliberam memoria ocupata de pointer
    cJSON_Delete(result);//eliberam memoria ocupata de obiectul JSON
    fclose(json_output);//inchidem fisierul
    free(outputFileName);//eliberam memoria ocupata
}

char *change_name_task3(char *inputFileName)//pe aceeasi idee ca la change_name_task1 numai ca adaptam extensia fisierului si numarul task-ului
{
    char *outputFileName, *prefix, nr1, nr2;
    outputFileName = calloc(100, sizeof(char));
    prefix = strstr(inputFileName, "board");
    nr1 = prefix[5];
    nr2 = prefix[6];
    sprintf(outputFileName, "output_task3_board%c%c.json", nr1, nr2);
    return outputFileName;
}

char *input_name(char *inputFileName)//extragem din inputFileName doar board##
{
    char *prefix;
    prefix = strstr(inputFileName, "board");
    return prefix;
}