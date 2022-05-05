/*
** database.c -- A simple database for storing movie records
*/

//  Returns:
//      0: operation successful
//      1: error during operation execution

// NOTES
// 1. Due to the simplicity of the problem domain, possible optimizations were
//  deliberately ignored in favor of an easily readable code.
// 2. Database locking hasn't been implemented. Therefore, receiving simultaneous
//  requests may lead to inconsistency since the server is threaded.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "movie.dat"
#define DB_SIZE 10

typedef struct
{
    u_int32_t id;
    char title[70];
    char genres[120];
    char director[50];
    char date[5];
} movie;

int init_db(){
    FILE *datafile;

    datafile = fopen(FILENAME, "r");
    if (datafile == NULL)
    {
        fprintf(stderr, "table does not exist yet, creating database...\n");
        datafile = fopen(FILENAME, "w+");
        if (datafile == NULL)
        {
            fprintf(stderr, "failed creating database\n");
            return 1;
        }
        fprintf(stderr, "database created!\n");
        fclose(datafile);
    }

    return 0;
}

int create(movie new_record)
{
    FILE *datafile;

    // Check if the new record id is valid (unique) before saving it
    datafile = fopen(FILENAME, "r");
    if (datafile == NULL)
    {
        fprintf(stderr, "database: fopen() failed\n");
        return 1;
    }

    movie record;
    while (fread(&record, sizeof(movie), 1, datafile))
        if (record.id == new_record.id)
            return 1;

    fclose(datafile);

    // Save new record
    datafile = fopen(FILENAME, "a");
    if (fwrite(&new_record, sizeof(movie), 1, datafile) != 1)
    {
        fprintf(stderr, "database: fwrite() failed\n");
        return 1;
    }

    fclose(datafile);

    return 0;
}

int destroy(u_int32_t id)
{
    FILE *datafile;
    movie remaining_records[DB_SIZE];

    // Load all records from datafile, except the ones that match the given id
    datafile = fopen(FILENAME, "r");
    if (datafile == NULL)
    {
        fprintf(stderr, "database: fopen() failed\n");
        return 1;
    }

    movie record;
    int response = 1, i = 0;
    while (fread(&record, sizeof(movie), 1, datafile))
    {
        if (record.id == id)
            // Note that this condition should always happen once, and only once
            // Otherwise, the database has already been damaged with inconsistency
            response = 0;
        else
        {
            remaining_records[i] = record;
            i++;
        }
    }

    fclose(datafile);

    if (response)
        // Return early to avoid unnecessary database writing
        return response;

    // Save only the remaining records
    datafile = fopen(FILENAME, "w");
    if (fwrite(&remaining_records, sizeof(movie), i, datafile) != i)
    {
        fprintf(stderr, "database: fwrite() failed\n");
        return 1;
    }

    fclose(datafile);

    return response;
}

int retrieve(movie *record, u_int32_t id)
// Fill the given record pointer with the database info
{
    // Caller should allocate record
    FILE *datafile;

    // Load records from datafile until find or endof
    datafile = fopen(FILENAME, "r");
    if (datafile == NULL)
    {
        fprintf(stderr, "database: fopen() failed\n");
        return 1;
    }

    int status = 1;
    while (fread(record, sizeof(movie), 1, datafile))
    {
        if (record->id == id)
        {
            status = 0;
            break;
        }
    }

    fclose(datafile);

    return status;
}

int list(movie *records)
// Fill the given records pointer with the database info
{
    FILE *datafile;

    // Load all records from datafile
    datafile = fopen(FILENAME, "r");
    if (datafile == NULL)
    {
        fprintf(stderr, "database: fopen() failed\n");
        return 1;
    }

    movie record;
    int i = 0;
    while (fread(&record, sizeof(movie), 1, datafile))
    {
        records[i] = record;
        i++;
    }

    fclose(datafile);

    return 0;
}

int list_filtered_by_genre(movie *records, char *genre)
// Fill the given records pointer with the database info
{
    // Get all records from datafile
    if (list(records))
        return 1;

    // Remove movies without the requested genre
    int i, j;
    for (i = 0; (i < DB_SIZE) && (records[i].id != 0); i++)
    {
        if (strstr(records[i].genres, genre) == NULL)
        {
            for (j = i; (j < DB_SIZE) && (records[j].id != 0); j++)
            {
                if ((j + 1 < DB_SIZE) && (records[j + 1].id != 0))
                    // Copy next element
                    records[j] = records[j + 1];
                else
                {
                    // Set to empty since there is no next element
                    movie empty_record = {0};
                    records[j] = empty_record;
                    break;
                }
            }
        }
    }

    return 0;
}

int add_gender_to_record(u_int32_t id, char *genre)
{
    FILE *datafile;
    movie records[DB_SIZE];

    // Load all records from datafile
    datafile = fopen(FILENAME, "r");
    if (datafile == NULL)
    {
        fprintf(stderr, "database: fopen() failed\n");
        return 1;
    }

    movie record;
    int response = 1, i = 0;
    while (fread(&record, sizeof(movie), 1, datafile))
    {
        if (record.id == id)
        {
            // Note that this condition should always happen once, and only once
            // Otherwise, the database has already been damaged with inconsistency
            // record.genres
            if (strlen(record.genres) + strlen(genre) < 120)
            {
                // Append genre
                response = 0;
                // FIX this shit not appending correctly
                strcat(record.genres, ";");
                strcat(record.genres, genre);
            }
            else
                // Cannot append genre, since there is no space left
                break;
        }
        records[i] = record;
        i++;
    }

    fclose(datafile);

    if (response)
        // Return early to avoid unnecessary database writing
        return response;

    // Save only the remaining records
    datafile = fopen(FILENAME, "w");
    if (fwrite(&records, sizeof(movie), i, datafile) != i)
    {
        fprintf(stderr, "database: fwrite() failed\n");
        return 1;
    }

    fclose(datafile);

    return response;
}