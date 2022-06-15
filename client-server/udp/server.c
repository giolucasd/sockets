/*
** server.c -- A datagram sockets server demo
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

#define MAXDATASIZE 4096 // Max number of bytes we can get at once

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

    // Main accept loop
    while(1){
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
                perror("server: socket");
                continue;
            }

            if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
            {
                close(sockfd);
                perror("server: bind");
                continue;
            }

            break;
        }

        freeaddrinfo(servinfo); // All done with this structure

        if (p == NULL)
        {
            fprintf(stderr, "server: failed to bind socket\n");
            return 2;
        }

        printf("server: waiting to recvfrom...\n");

        int numbytes;
        struct sockaddr_storage client_addr;
        socklen_t client_addr_len = sizeof client_addr;
        char request_buffer[MAXDATASIZE];
        if (
            (
                numbytes = recvfrom(
                    sockfd,
                    request_buffer,
                    MAXDATASIZE - 1,
                    0,
                    (struct sockaddr *)&client_addr,
                    &client_addr_len)) == -1)
        {
            perror("recvfrom");
            exit(1);
        }

        char addr_str[INET6_ADDRSTRLEN];
        printf(
            "server: got packet from %s\n",
            inet_ntop(
                client_addr.ss_family,
                get_in_addr((struct sockaddr *)&client_addr),
                addr_str,
                sizeof addr_str));

        request_buffer[numbytes] = '\0';

        // Process request generating a response
        response res = operation_router(request_buffer);

        if (
            sendto(
                sockfd,
                res.response_data,
                res.response_size,
                0,
                (struct sockaddr *)&client_addr,
                client_addr_len) == -1)
            perror("send");

        free_response(res); // All done with this structure
        close(sockfd);
    }

    return 0;
}