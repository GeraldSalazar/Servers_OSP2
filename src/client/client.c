#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../../include/shared.h"

// cliente  <ip>  <puerto>  <imagen>  <N−threads>  <N−ciclos>

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    char message[100];

    char* ip, *imageName;
    int port, nThreads, nCycles;
    if(argc != 6){
        printf("Error! Usage: %s  <ip>  <port>  <imagePath>  <N−threads>  <N−cycles> \n", argv[0]);
        return -1;
    }
    ip = argv[1];               // server IP address
    imageName = argv[3];        // path and name of the image to send
    port = atoi(argv[2]);       // server port number
    nThreads = atoi(argv[4]);   // number of threads doing requests to server
    nCycles = atoi(argv[5]);    // number of times a thread should make a request

    printf("Params received:\n  <ip>: %s\n  <port>: %d\n  <imagePath>: %s\n  <N−threads>: %d\n  <N−cycles>: %d \n", ip, port, imageName, nThreads, nCycles);

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    // Set server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP); // Replace with server IP address
    server_addr.sin_port = htons(PORT); // Replace with server port number

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    // // Send message to server
    // strcpy(message, "Hello server!");
    // if (send(sockfd, message, strlen(message), 0) < 0) {
    //     perror("Error sending message");
    //     exit(EXIT_FAILURE);
    // }

    // Receive response from server
    memset(message, 0, sizeof(message));
    if (recv(sockfd, message, sizeof(message), 0) < 0) {
        perror("Error receiving response");
        exit(EXIT_FAILURE);
    }

    printf("Server response: %s\n", message);

    // Close socket
    close(sockfd);

    return 0;
}
