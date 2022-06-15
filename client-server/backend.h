/*
** backend.c -- Backend for operation based processing
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "database.h"

typedef struct
{
    char *response_data;
    int response_size;
} response;

void free_response(response res)
{
    free(res.response_data);
}

void operation1(char *response_data, char *request_data)
{
    movie record = {0};

    // Fill argument record with given request data
    memcpy(&record.id, &request_data[1], sizeof(u_int32_t));
    strncpy(record.title, &request_data[5], 70);
    strncpy(record.genres, &request_data[75], 120);
    strncpy(record.director, &request_data[195], 50);
    strncpy(record.date, &request_data[245], 5);

    if (create(record) != 0)
    {
        response_data[0] = 1;
        return;
    }

    response_data[0] = 0;
}

void operation2(char *response_data, char *request_data)
{
    u_int32_t id;
    char genre[15];

    // Fill arguments with given request data
    memcpy(&id, &request_data[1], sizeof(u_int32_t));
    strncpy(genre, &request_data[5], 15);

    if (add_gender_to_record(id, genre) != 0)
    {
        response_data[0] = 1;
        return;
    }

    response_data[0] = 0;
}

void operation3(char *response_data)
{
    movie records[DB_SIZE];
    memset(records, 0, DB_SIZE * sizeof(movie));

    if (list(records) != 0)
    {
        response_data[0] = 1;
        return;
    }

    // Fill response data with the processed record relevant fields
    response_data[0] = 0;
    int i = 0;
    while (records[i].id != 0)
    {
        memcpy(&response_data[(74 * i) + 1], &records[i].id, sizeof(u_int32_t));
        strncpy(&response_data[(74 * i) + 5], records[i].title, 70);
        i++;
    }

    response_data[(74 * i) + 1] = '\n';
}

void operation4(char *response_data, char *request_data)
{
    movie records[DB_SIZE];
    memset(records, 0, DB_SIZE * sizeof(movie));
    char genre[15];

    strncpy(genre, &request_data[1], 15);

    if (list_filtered_by_genre(records, genre) != 0)
    {
        response_data[0] = 1;
        return;
    }

    // Fill response data with the processed record relevant fields
    response_data[0] = 0;
    int i = 0;
    while (records[i].id != 0)
    {
        strncpy(&response_data[(125 * i) + 1], records[i].title, 70);
        strncpy(&response_data[(125 * i) + 71], records[i].director, 50);
        strncpy(&response_data[(125 * i) + 121], records[i].date, 5);
        i++;
    }
    response_data[(125 * i) + 1] = '\n';
}

void operation5(char *response_data)
{
    movie records[DB_SIZE];
    memset(records, 0, DB_SIZE * sizeof(movie));

    if (list(records) != 0)
    {
        response_data[0] = 1;
        return;
    }

    // Fill response data with the processed record relevant fields
    response_data[0] = 0;
    int i = 0;
    while (records[i].id != 0)
    {
        memcpy(&response_data[(249 * i) + 1], &records[i].id, sizeof(u_int32_t));
        strncpy(&response_data[(249 * i) + 5], records[i].title, 70);
        strncpy(&response_data[(249 * i) + 75], records[i].genres, 120);
        strncpy(&response_data[(249 * i) + 195], records[i].director, 50);
        strncpy(&response_data[(249 * i) + 245], records[i].date, 5);
        i++;
    }
    response_data[(249 * i) + 1] = '\n';
}

void operation6(char *response_data, char *request_data)
{
    u_int32_t id;
    movie record;

    memcpy(&id, &request_data[1], sizeof(u_int32_t));

    if (retrieve(&record, id) != 0)
    {
        response_data[0] = 1;
        return;
    }

    // Fill response data with the processed record relevant fields
    response_data[0] = 0;
    memcpy(&response_data[1], &record.id, sizeof(u_int32_t));
    strncpy(&response_data[5], record.title, 70);
    strncpy(&response_data[75], record.genres, 120);
    strncpy(&response_data[195], record.director, 50);
    strncpy(&response_data[245], record.date, 5);
}

void operation7(char *response_data, char *request_data)
{
    u_int32_t id;

    memcpy(&id, &request_data[1], sizeof(u_int32_t));

    if (destroy(id) != 0)
    {
        response_data[0] = 1;
        return;
    }

    response_data[0] = 0;
}

response operation_router(char *request_data)
// Return response struct, the caller should free it after using
{
    response res;

    char operation = request_data[0];
    switch (operation)
    {
    case 1:
        res.response_size = 1 * sizeof(char);
        res.response_data = (char *)malloc(res.response_size);
        operation1(res.response_data, request_data);
        break;

    case 2:
        res.response_size = 1 * sizeof(char);
        res.response_data = (char *)malloc(res.response_size);
        operation2(res.response_data, request_data);
        break;

    case 3:
        res.response_size = (2 + 74 * DB_SIZE) * sizeof(char);
        res.response_data = (char *)malloc(res.response_size);
        operation3(res.response_data);
        break;

    case 4:
        res.response_size = (2 + 125 * DB_SIZE) * sizeof(char);
        res.response_data = (char *)malloc(res.response_size);
        operation4(res.response_data, request_data);
        break;

    case 5:
        res.response_size = (2 + 249 * DB_SIZE) * sizeof(char);
        res.response_data = (char *)malloc(res.response_size);
        operation5(res.response_data);
        break;

    case 6:
        res.response_size = 250 * sizeof(char);
        res.response_data = (char *)malloc(res.response_size);
        operation6(res.response_data, request_data);
        break;

    case 7:
        res.response_size = 1 * sizeof(char);
        res.response_data = (char *)malloc(res.response_size);
        operation7(res.response_data, request_data);
        break;

    default:
        res.response_size = 1 * sizeof(char);
        res.response_data = (char *)malloc(res.response_size);
        res.response_data[0] = 1;
    }

    return res;
}