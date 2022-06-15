/*
** renderer.c -- Renderer for operation based response interface
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "database.h"

void render1(char *response_data)
{
    if (response_data[0] == 0)
    {
        printf("Filme criado com sucesso!\n");
        return;
    }
    printf("Falha ao tentar criar o filme.\n");
}

void render2(char *response_data)
{
    if (response_data[0] == 0)
    {
        printf("Genêro acrescentado com sucesso!\n");
        return;
    }
    printf("Falha ao tentar acrescentar gênero.\n");
}

void render3(char *response_data)
{
    if (response_data[0] != 0)
    {
        printf("Falha ao consultar filmes.\n");
        return;
    }

    u_int32_t id;
    char title[70];

    int i = 0;
    do
    {
        if (response_data[(74 * i) + 1] == '\n')
            break;

        memcpy(&id, &response_data[(74 * i) + 1], sizeof(u_int32_t));
        strncpy(title, &response_data[(74 * i) + 5], 70);

        printf("Filme:\n");
        printf("\tid: %u\n", id);
        printf("\ttitle: %s\n", title);

        i++;
    } while (i < DB_SIZE);
}

void render4(char *response_data)
{
    if (response_data[0] != 0)
    {
        printf("Falha ao consultar filmes para este gênero.\n");
        return;
    }
    char title[70], director[50], date[5];

    int i = 0;
    do
    {
        if (response_data[(125 * i) + 1] == '\n')
            break;

        strncpy(title, &response_data[(125 * i) + 1], 70);
        strncpy(director, &response_data[(125 * i) + 71], 50);
        strncpy(date, &response_data[(125 * i) + 121], 5);

        printf("Filme:\n");
        printf("\ttitle: %s\n", title);
        printf("\tdirector: %s\n", director);
        printf("\tdate: %s\n", date);

        i++;
    } while (i < DB_SIZE);
}

void render5(char *response_data)
{
    if (response_data[0] != 0)
    {
        printf("Falha ao consultar filmes.\n");
        return;
    }
    u_int32_t id;
    char title[70], genres[120], director[50], date[5];

    int i = 0;
    do
    {
        if (response_data[(249 * i) + 1] == '\n')
            break;

        memcpy(&id, &response_data[(249 * i) + 1], sizeof(u_int32_t));
        strncpy(title, &response_data[(249 * i) + 5], 70);
        strncpy(genres, &response_data[(249 * i) + 75], 120);
        strncpy(director, &response_data[(249 * i) + 195], 50);
        strncpy(date, &response_data[(249 * i) + 245], 5);

        printf("Filme:\n");
        printf("\tid: %u\n", id);
        printf("\ttitle: %s\n", title);
        printf("\tgenres: %s\n", genres);
        printf("\tdirector: %s\n", director);
        printf("\tdate: %s\n", date);

        i++;
    } while (i < DB_SIZE);
}

void render6(char *response_data)
{
    if (response_data[0] != 0)
    {
        printf("Falha ao consultar o filme.\n");
        return;
    }

    u_int32_t id;
    char title[70], genres[120], director[50], date[5];

    memcpy(&id, &response_data[1], sizeof(u_int32_t));
    strncpy(title, &response_data[5], 70);
    strncpy(genres, &response_data[75], 120);
    strncpy(director, &response_data[195], 50);
    strncpy(date, &response_data[245], 5);

    printf("Filme:\n");
    printf("\tid: %u\n", id);
    printf("\ttitle: %s\n", title);
    printf("\tgenres: %s\n", genres);
    printf("\tdirector: %s\n", director);
    printf("\tdate: %s\n", date);
}

void render7(char *response_data)
{
    if (response_data[0] == 0)
    {
        printf("Filme removido com sucesso!\n");
        return;
    }
    printf("Falha ao tentar remover o filme.\n");
}

void render_response(char operation, char *response_data)
// Render response accordingly to the operation
{
    int response_size;

    printf("\nResposta:\n\n");

    switch (operation)
    {
    case 1:
        response_size = 1 * sizeof(char);
        render1(response_data);
        break;

    case 2:
        response_size = 1 * sizeof(char);
        render2(response_data);
        break;

    case 3:
        response_size = (2 + 74 * DB_SIZE) * sizeof(char);
        render3(response_data);
        break;

    case 4:
        response_size = (2 + 125 * DB_SIZE) * sizeof(char);
        render4(response_data);
        break;

    case 5:
        response_size = (2 + 249 * DB_SIZE) * sizeof(char);
        render5(response_data);
        break;

    case 6:
        response_size = 250 * sizeof(char);
        render6(response_data);
        break;

    case 7:
        response_size = 1 * sizeof(char);
        render7(response_data);
        break;

    default:
        printf("Operação inválida!\n");
    }

    printf("\n\n********************************************************************************************\n\n");
}