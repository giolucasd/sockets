/*
** interface.c -- Interface for operation based interface
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "helper.h"

typedef struct
{
    char *request_data;
    int request_size;
} request;

void free_request(request req)
{
    free(req.request_data);
}

void operation1(char *request_data)
{
    request_data[0] = 1;

    printf("Insira o id do filme a ser criado (deve ser único):\n");


    u_int32_t id;
    get_int_from_stdin(&id);

    memcpy(&request_data[1], &id, sizeof(u_int32_t));

    printf("Insira o título do filme (deve ter no máximo 69 caracteres):\n");
    get_str_from_stdin(&request_data[5], 70);

    printf("Insira os gêneros do filme separados por ';' (cada gênero pode ter no máximo 14 caracteres):\n");
    get_str_from_stdin(&request_data[75], 120);

    printf("Insira o diretor do filme (deve ter no máximo 49 caracteres):\n");
    get_str_from_stdin(&request_data[195], 50);

    printf("Insira o ano de lançamento do filme (deve ter 4 caracteres):\n");
    get_str_from_stdin(&request_data[245], 5);
}

void operation2(char *request_data)
{
    request_data[0] = 2;

    printf("Insira o id do filme ao qual será acrescentado o gênero:\n");

    u_int32_t id;
    get_int_from_stdin(&id);

    memcpy(&request_data[1], &id, sizeof(u_int32_t));

    printf("Insira o gênero a ser adicionado (deve ter no máximo 14 caracteres):\n");
    get_str_from_stdin(&request_data[5], 15);
}


void operation3(char *request_data)
{
    request_data[0] = 3;
}

void operation4(char *request_data)
{
    request_data[0] = 4;
    
    printf("Insira o gênero a ser buscado (deve ter no máximo 14 caracteres):\n");
    clear_input_buffer();
    get_str_from_stdin(&request_data[1], 15);
}

void operation5(char *request_data)
{
    request_data[0] = 5;
}

void operation6(char *request_data)
{
    request_data[0] = 6;

    printf("Insira o id do filme a buscar:\n");

    u_int32_t id;
    get_int_from_stdin(&id);

    memcpy(&request_data[1], &id, sizeof(u_int32_t));
}

void operation7(char *request_data)
{
    request_data[0] = 7;
    
    printf("Insira o id do filme a deletar:\n");

    u_int32_t id;
    get_int_from_stdin(&id);

    memcpy(&request_data[1], &id, sizeof(u_int32_t));
}

request interface_handler()
// Return request struct, the caller should free it after using
{
    request req;
    char operation;

    printf("Operações disponíveis:\n");
    printf("\t* 1 - cadastrar um novo filme;\n");
    printf("\t* 2 - acrescentar um novo gênero em um filme;\n");
    printf("\t* 3 - listar todos os títulos, junto a seus respectivos identificadores;\n");
    printf("\t* 4 - listar informações de todos os filmes de um determinado gênero;\n");
    printf("\t* 5 - listar todas as informações de todos os filmes;\n");
    printf("\t* 6 - listar todas as informações de um filme;\n");
    printf("\t* 7 - remover um filme.\n");
    printf("Insira o código da operação desejada:\n");
    
    get_operation:

    operation = getchar();

    switch (operation)
    {
    case '1':
        req.request_size = 250 * sizeof(char);
        req.request_data = (char *)malloc(req.request_size);
        operation1(req.request_data);
        break;

    case '2':
        req.request_size = 20 * sizeof(char);
        req.request_data = (char *)malloc(req.request_size);
        operation2(req.request_data);
        break;

    case '3':
        req.request_size = 1 * sizeof(char);
        req.request_data = (char *)malloc(req.request_size);
        operation3(req.request_data);
        break;

    case '4':
        req.request_size = 16 * sizeof(char);
        req.request_data = (char *)malloc(req.request_size);
        operation4(req.request_data);
        break;

    case '5':
        req.request_size = 1 * sizeof(char);
        req.request_data = (char *)malloc(req.request_size);
        operation5(req.request_data);
        break;

    case '6':
        req.request_size = 5 * sizeof(char);
        req.request_data = (char *)malloc(req.request_size);
        operation6(req.request_data);
        break;

    case '7':
        req.request_size = 5 * sizeof(char);
        req.request_data = (char *)malloc(req.request_size);
        operation7(req.request_data);
        break;

    default:
        printf("Insira uma operação válida!\n");
        goto get_operation;
    }

    return req;
}