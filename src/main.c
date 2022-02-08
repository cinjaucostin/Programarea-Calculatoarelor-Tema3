#include <stdio.h>
#include "bmp_header.h"
#include "cJSON.h"
#include "utils.h"

void extract(char *inputFileName)
{
    FILE *file, *outputFile_task1, *outputFile_task2;
    char *j_str, *outputFileName_task1, *outputFileName_task2;
    int lungime, k = 0, **number_matrix;
    int ok;
    unsigned char bmppad[3] = {0, 0, 0};//vectorul pe care il vom folosi pentru paddind(cand vom scrie in fisierul binar matricea de pixeli)

    outputFileName_task1 = change_name_task1(inputFileName);//apelam functia pentru a obtine numele fisierului de output specific taskului 1
    outputFileName_task2 = change_name_task2(inputFileName);//apelam functia pentru a obtine numele fisierului de output specific taskului 2
    file = fopen(inputFileName, "r");
    outputFile_task1 = fopen(outputFileName_task1, "wb");
    outputFile_task2 = fopen(outputFileName_task2, "wb");
    free(outputFileName_task1);//eliberam memoria ocupata de pointer
    free(outputFileName_task2);//eliberam memoria ocupata de pointer
    fseek(file, 0, SEEK_END);//pentru a putea afla lungimea fisierului json
    lungime = ftell(file);
    fseek(file, 0, SEEK_SET);
    j_str = calloc((lungime + 1), sizeof(char));//alocam memorie pentru pointerul in care vom salva datele din fisierul json
    fread(j_str, sizeof(char), lungime, file);//citim datele din fisierul json si le introducem in pointer

    cJSON *parsare = cJSON_Parse(j_str);//parsam din pointerul j_str cu ajutorul functiei cJSON_Parse
    cJSON *file_header = cJSON_GetObjectItem(parsare, "file_header");//extragem obiectul (partea din pointer) care este descris prin campul "file_header"
    bmp_fileheader *header1;//in liniile urmatoare introducem fiecare informatie despre fileheader in fiecare camp specific acesteia din cadrul structurii
    header1 = malloc(sizeof(bmp_fileheader));
    header1->imageDataOffset = cJSON_GetObjectItem(file_header, "offset")->valueint;
    header1->fileMarker1 = cJSON_GetObjectItem(file_header, "signature")->valuestring[0];
    header1->fileMarker2 = cJSON_GetObjectItem(file_header, "signature")->valuestring[1];
    header1->unused1 = cJSON_GetObjectItem(file_header, "reserved")->valueint;
    header1->unused2 = cJSON_GetObjectItem(file_header, "reserved")->valueint;
    header1->bfSize = cJSON_GetObjectItem(file_header, "file_size")->valueint;

    cJSON *info_header = cJSON_GetObjectItem(parsare, "info_header");//acelasi lucru pe care l-am facut si pentru fileheader
    bmp_infoheader *header2;
    header2 = malloc(sizeof(bmp_infoheader));
    header2->biClrUsed = cJSON_GetObjectItem(info_header, "colors_used")->valueint;
    header2->biSize = cJSON_GetObjectItem(info_header, "size")->valueint;
    header2->biClrImportant = cJSON_GetObjectItem(info_header, "colors_important")->valueint;
    header2->biSizeImage = cJSON_GetObjectItem(info_header, "image_size")->valueint;
    header2->biYPelsPerMeter = cJSON_GetObjectItem(info_header, "y_pixels_per_meter")->valueint;
    header2->biXPelsPerMeter = cJSON_GetObjectItem(info_header, "x_pixels_per_meter")->valueint;
    header2->width = cJSON_GetObjectItem(info_header, "width")->valueint;
    header2->planes = cJSON_GetObjectItem(info_header, "planes")->valueint;
    header2->bitPix = cJSON_GetObjectItem(info_header, "bit_count")->valueint;
    header2->biCompression = cJSON_GetObjectItem(info_header, "compression")->valueint;
    header2->height = cJSON_GetObjectItem(info_header, "height")->valueint;

    cJSON *map = cJSON_GetObjectItem(parsare, "bitmap");//portiunea din parsare specifica bitmap-ului
    pixel **pixels_matrix;//declaram o matrice de pixeli in care vom introduce informatiile prezente in bitmap
    pixels_matrix = calloc(header2->height, sizeof(pixel *));//alocam memorie pentru matricea de pixeli
    for (int i = 0; i < header2->height; i++)
        pixels_matrix[i] = calloc(header2->width, sizeof(pixel *));
    for (int i = 0; i < header2->height; i++)
    {
        for (int j = 0; j < header2->width; j++)//introducem in structura pixel de coordonate i si j valorile specifice pentru campul B(Blue), G(Green), R(Red)
        {
            //in fisier acesta valori ne sunt date in ordine inversa(BGR) fata de formatul normal(RGB) astfel incat pe pozitia 3k regasim valoarea pentru blue, 3k+1 valoarea pentru green si pe 3k+2 valorea pentru red 
            pixels_matrix[i][j].B = cJSON_GetArrayItem(map, 3 * k)->valueint;
            pixels_matrix[i][j].G = cJSON_GetArrayItem(map, 3 * k + 1)->valueint;
            pixels_matrix[i][j].R = cJSON_GetArrayItem(map, 3 * k + 2)->valueint;
            k++;
        }
    }
    cJSON_Delete(parsare);//eliberam toata memoria consumata de tot declaratiile facute cu cJSON(au legatura cu parsarea)
    //introducem informatiile despre header atat in fisierul de output pentru task1 cat si in fisierul de output pentru task2
    fwrite(header1, sizeof(*header1), 1, outputFile_task1);
    fwrite(header2, sizeof(*header2), 1, outputFile_task1);
    fwrite(header1, sizeof(*header1), 1, outputFile_task2);
    fwrite(header2, sizeof(*header2), 1, outputFile_task2);
    for (int i = header2->height - 1; i >= 0; i--)//am mers cu i-ul descrescator deoarece imaginea ar fi rasturnata daca as fi ales calea crescatoare
    {
        for (int j = 0; j < header2->width; j++)
            fwrite(&pixels_matrix[i][j], sizeof(pixel), 1, outputFile_task1);//scriem in fisierul de output matricea de pixeli
        fwrite(bmppad, 1, (4 - (header2->height * 3) % 4) % 4, outputFile_task1);//padding-ul aferent(fara acest padding imaginea ar avea pixelii decalati)
    }

    for (int i = header2->height - 1; i >= 0; i--)//in continuare va trebui sa intoarcem fiecare numar in parte din matrice deoarece acesta se vede initial in oglinda(ideea in sine se rezuma la faptul ca vom parcurge fiecare portiune de matrice specifica fiecarui numar)
    {
        for (int j = 2; j < header2->width; j += 8)//am studiat putin hint-ul oferit pe ocw si am observat ca trebuie sa plec cu j-ul de la pozitia 2 si sa merg cu el din 8 in 8
        {
            //in continuare schimbam pixelii aflati la lungime egala fata de margini(pozitia 0 cu 4 si 1 cu 3)
            unsigned char aux = pixels_matrix[i][j].R;
            pixels_matrix[i][j].R = pixels_matrix[i][j + 4].R;
            pixels_matrix[i][j + 4].R = aux;
            aux = pixels_matrix[i][j].G;
            pixels_matrix[i][j].G = pixels_matrix[i][j + 4].G;
            pixels_matrix[i][j + 4].G = aux;
            aux = pixels_matrix[i][j].B;
            pixels_matrix[i][j].B = pixels_matrix[i][j + 4].B;
            pixels_matrix[i][j + 4].B = aux;
            aux = pixels_matrix[i][j + 1].R;
            pixels_matrix[i][j + 1].R = pixels_matrix[i][j + 3].R;
            pixels_matrix[i][j + 3].R = aux;
            aux = pixels_matrix[i][j + 1].G;
            pixels_matrix[i][j + 1].G = pixels_matrix[i][j + 3].G;
            pixels_matrix[i][j + 3].G = aux;
            aux = pixels_matrix[i][j + 1].B;
            pixels_matrix[i][j + 1].B = pixels_matrix[i][j + 3].B;
            pixels_matrix[i][j + 3].B = aux;
        }
    }

    for (int i = header2->height - 1; i >= 0; i--)//introducem matricea de pixeli alaturi de padding-ul aferent in fisierul binar specific taskului 2
    {
        for (int j = 0; j < header2->width; j++)
            fwrite(&pixels_matrix[i][j], sizeof(pixel), 1, outputFile_task2);
        fwrite(bmppad, 1, (4 - (header2->height * 3) % 4) % 4, outputFile_task2);
    }
    //alocam memorie pentru matricea number_matrix in care vom introduce numerele ce se disting din fisierul cu extensia bmp(imaginea tablei de sudoku)
    number_matrix = calloc(9, sizeof(int *));
    for (int i = 0; i < 9; i++)
        number_matrix[i] = calloc(9, sizeof(int));
    for (int i = 0; i < 9; i++)//vom avea doua for-uri de la 0 la 9 pentru ca tabla noastra este de format 9x9 numere
    {
        for (int j = 0; j < 9; j++)
        {
            int nr = 0;
            for (int k = 8 * i + 2; k <= 8 * i + 6; k++)//de ex primul numar se desfasoara pentru k de la 2 la 6(pentru liniile de la 2 la 6)
            {
                for (int l = 8 * j + 2; l <= 8 * j + 6; l++)//acelasi lucru ca si la linii doar ca pentru coloane(de la coloana 2 la coloana 6)
                {
                    if (pixels_matrix[k][l].R == 255 && pixels_matrix[k][l].G == 175 && pixels_matrix[k][l].B == 175)
                    {
                        nr++; //numaram cati pixeli roz avem pe acea portiune de matrice de pixeli
                    }
                }
            }
            if (nr == 5) //verificare pentru 1
            {
                number_matrix[i][j] = 1;
            }
            else if (nr == 17)
            {
                if (pixels_matrix[8 * i + 5][8 * j + 2].R == 255 && pixels_matrix[8 * i + 5][8 * j + 2].G == 175 && pixels_matrix[8 * i + 5][8 * j + 2].B == 175) //verificare pentru 2
                    number_matrix[i][j] = 2;
                else
                {
                    if (pixels_matrix[8 * i + 3][8 * j + 6].R == 255 && pixels_matrix[8 * i + 3][8 * j + 6].G == 175 && pixels_matrix[8 * i + 3][8 * j + 6].B == 175) //verificare pentru 3
                        number_matrix[i][j] = 3;
                    else // verificare pentru 5
                        number_matrix[i][j] = 5;
                }
            }
            else if (nr == 11) //verificare pentru 4
                number_matrix[i][j] = 4;
            else if (nr == 18)
            {
                if (pixels_matrix[8 * i + 3][8 * j + 6].R == 255 && pixels_matrix[8 * i + 3][8 * j + 6].G == 175 && pixels_matrix[8 * i + 3][8 * j + 6].B == 175) //verificare pentru 9
                    number_matrix[i][j] = 9;
                else //verificare pentru 6
                    number_matrix[i][j] = 6;
            }
            else if (nr == 9) //verificare pentru 7
                number_matrix[i][j] = 7;
            else if (nr == 19) //verificare pentru 8
                number_matrix[i][j] = 8;
        }
    }
    if (verify_column(number_matrix) && verify_square(number_matrix) && verify_line(number_matrix))//inseamna ca toate regulile jocului sudoku sunt respectate
        ok = 1;
    else
        ok = 0;
    create_json(ok,inputFileName);//apelam functia pentru task3
    //Eliberarea memoriei utilizate
    for (int i = 0; i < header2->height; i++) //eliberarea memoriei ocupata de matricea pixels_matrix
        free(pixels_matrix[i]);
    free(pixels_matrix);
    for (int i = 0; i < 9; i++) //eliberarea memoriei ocupata de matricea number_matrix
        free(number_matrix[i]);
    free(number_matrix);
    free(j_str);
    free(header1);
    free(header2);
    //Inchiderea fisierelor pe care le-am folosit
    fclose(outputFile_task1);
    fclose(outputFile_task2);
    fclose(file);
}

int main(int argc, char **argcv)
{
    if (argc != 3)
        return -1;

    char *inputFileName = strdup(argcv[1]);
    extract(inputFileName);
    free(inputFileName);
    return 0;
}