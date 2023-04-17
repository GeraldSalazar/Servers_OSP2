#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../../include/shared.h"



// struct sockaddr_in {
//     sa_family_t sin_family; // Address family (should always be AF_INET)
//     in_port_t sin_port;     // Port number (in network byte order)
//     struct in_addr sin_addr;// IP address (in network byte order)
//     char sin_zero[8];       // Padding to make the struct the same size as struct sockaddr
// };

// network byte order  ->  the most significant byte is transmitted first (big-endian)

// SO_REUSEPORT  ->  allows multiple sockets to bind to the same IP address and port number combination.
// SO_REUSEADDR  ->  allows a socket to be bound to a local address that is already in use.

int main(int argc, char const *argv[])
{

    int socket_fd, new_socket;
    int opt = 1;
    char buffer[1024] = {0};

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;               //represents the address family for IPv4
    address.sin_addr.s_addr = INADDR_ANY;       //any IP address on the local machine.
    address.sin_port = htons( PORT );           //takes a 16-bit integer value in host byte order as its argument, and returns the corresponding value in network byte order.

    // Create socket file descriptor
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == 0) {
        perror("Error with socket call");
        exit(EXIT_FAILURE);
    }
    printf("socket_fd: %d \n", socket_fd);

    // Setting socket configs
    int optRes = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (optRes < 0) {
        perror("Error with setsockopt call");
        exit(EXIT_FAILURE);
    }
    printf("optRes: %d \n", optRes);

    // Bind the socket to the specified address and port
    int bindRes = bind(socket_fd, (struct sockaddr *)&address, sizeof(address));
    if (bindRes<0) {
        perror("Error with bind call");
        exit(EXIT_FAILURE);
    }
    printf("bindRes: %d \n" , bindRes);

    // Listen for incoming connections
    int queue_max = 4;
    if (listen(socket_fd, queue_max) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Display server IP address
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(socket_fd, (struct sockaddr *)&sin, &len) == -1) {
        perror("getsockname");
        exit(EXIT_FAILURE);
    }


    char *hello = "Hello from the server...";
    while(1)
    {
        printf("Waiting for a connection on port %d. IP: %s\n", PORT, inet_ntoa(sin.sin_addr));
        fflush(stdout);
        // Accept incoming connections and handle them
        if ((new_socket = accept(socket_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        send(new_socket, hello, strlen(hello), 0);
        printf("Message sent\n");
        close(new_socket);
    }
    return 0;
}
