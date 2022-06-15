/*
** utils.h -- Socket helper functions and definitions
*/

#include <netinet/in.h>
#include <sys/socket.h>

#define SERVER_PORT "3490" // The server port

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sock_addr)
{
    if (sock_addr->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sock_addr)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sock_addr)->sin6_addr);
}