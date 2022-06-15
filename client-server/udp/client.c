/*
** client.c -- a datagram "client" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../utils.h"
#include "../interface.h"
#include "../renderer.h"

#define MAXDATASIZE 4096 // Max number of bytes we can get at once

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: client hostname message\n");
        exit(1);
    }

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    int response_status;
    if ((response_status = getaddrinfo(argv[1], SERVER_PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(response_status));
        return 1;
    }

    // Loop through all the results and make a socket
    int sockfd;
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to create socket\n");
        return 2;
    }

    freeaddrinfo(servinfo); // All done with this structure

    request req;
    int numbytes;
    char response_buffer[MAXDATASIZE];

    // Standardize client CLI outputs
    printf("\n\n********************************************************************************************\n\n");

    while(1){
        req = interface_handler();

        if ((numbytes = sendto(
                sockfd,
                req.request_data,
                req.request_size,
                0,
                p->ai_addr,
                p->ai_addrlen
            )
        ) == -1)
        {
            perror("client: sendto");
            exit(1);
        }

        if ((numbytes = recvfrom(
                sockfd,
                response_buffer,
                MAXDATASIZE - 1,
                0,
                p->ai_addr,
                &(p->ai_addrlen)
            )
            ) == -1
        )
        {
            perror("recvfrom");
            exit(1);
        }

        response_buffer[numbytes] = '\0';

        render_response(req.request_data[0], response_buffer);

        free_request(req);  // All done with this structure

    }
    close(sockfd);

    return 0;
}