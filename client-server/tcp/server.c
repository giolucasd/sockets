/*
** server.c -- A stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3490" // The port users will be connecting to

#define BACKLOG 10 // How many pending connections queue will hold

void sigchld_handler(int sig)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;

    errno = saved_errno;
}

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sock_addr)
{
    if (sock_addr->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sock_addr)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sock_addr)->sin6_addr);
}

int main(void)
{
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    
    int response_status;
    if ((response_status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(response_status));
        return 1;
    }

    int reuse_addr = 1; // Allows other sockets to bind() to this port, unless there
                        // is an active listening socket bound to the port already.

    // Loop through all the results and bind to the first we can
    int sockfd; // Server listen on sock_fd
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
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
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    struct sigaction signal_action;
    signal_action.sa_handler = sigchld_handler; // Reap all dead processes
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &signal_action, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    struct sockaddr_storage client_addr; // Connector's address information
    int new_sockfd;                      // New connections are accepted on new_sockfd
    char addr_str[INET6_ADDRSTRLEN];
    while (1)
    { // Main accept() loop
        socklen_t client_addr_len = sizeof client_addr;
        new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (new_sockfd == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(
            client_addr.ss_family,
            get_in_addr((struct sockaddr *)&client_addr),
            addr_str,
            sizeof addr_str);
        printf("server: got connection from %s\n", addr_str);

        if (!fork())
        {                  // This is the child process
            close(sockfd); // Child doesn't need the listener
            if (send(new_sockfd, "Hello, world!", 13, 0) == -1)
                perror("send");
            close(new_sockfd);
            exit(0);
        }
        close(new_sockfd); // Parent doesn't need this
    }

    return 0;
}