/*
** listener.c -- A datagram sockets "server" demo
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
#include "../backend.h"

#define MAXBUFLEN 4096 // Max number of bytes we can get at once

int main(void)
{
    // Initialize database
    if (init_db())
        return 1;

    // Handle server comunication
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6; // Use specifically IPv6 because UDP is connectionless
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    int response_status;
    if ((response_status = getaddrinfo(NULL, SERVER_PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(response_status));
        return 1;
    }

    // Loop through all the results and bind to the first we can
    int sockfd;
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo); // All done with this structure

    printf("listener: waiting to recvfrom...\n");

    int numbytes;
    struct sockaddr_storage talker_addr;
    socklen_t talker_addr_len = sizeof talker_addr;
    char request_buffer[MAXBUFLEN];
    if ((numbytes = recvfrom(sockfd, request_buffer, MAXBUFLEN - 1, 0,
                             (struct sockaddr *)&talker_addr, &talker_addr_len)) == -1)
    {
        perror("recvfrom");
        exit(1);
    }

    char addr_str[INET6_ADDRSTRLEN];
    printf("listener: got packet from %s\n",
           inet_ntop(talker_addr.ss_family,
                     get_in_addr((struct sockaddr *)&talker_addr),
                     addr_str, sizeof addr_str));
    printf("listener: packet is %d bytes long\n", numbytes);
    request_buffer[numbytes] = '\0';
    printf("listener: packet contains \"%s\"\n", request_buffer);

    close(sockfd);

    return 0;
}