/*
** client.c -- A stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include "../utils.h"
#include "../interface.h"
#include "../renderer.h"

#define MAXDATASIZE 4096 // Max number of bytes we can get at once

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int response_status;
    if ((response_status = getaddrinfo(argv[1], SERVER_PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(response_status));
        return 1;
    }

    // Loop through all the results and connect to the first we can
    int sockfd;
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    char addr_str[INET6_ADDRSTRLEN];
    inet_ntop(
        p->ai_family,
        get_in_addr((struct sockaddr *)p->ai_addr),
        addr_str,
        sizeof addr_str);
    printf("client: connecting to %s\n", addr_str);

    freeaddrinfo(servinfo); // All done with this structure

    request req = interface_handler();

    if (send(sockfd, req.request_data, req.request_size, 0) == -1)
    {
        perror("send");
        exit(1);
    }

    int numbytes;
    char response_buffer[MAXDATASIZE];
    if ((numbytes = recv(sockfd, response_buffer, MAXDATASIZE - 1, 0)) == -1)
    {
        perror("recv");
        exit(1);
    }

    response_buffer[numbytes] = '\0';

    render_response(req.request_data[0], response_buffer);
    
    free_request(req);  // All done with this structure

    close(sockfd);

    return 0;
}