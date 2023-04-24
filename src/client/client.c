#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "../../include/shared.h"

// cliente  <ip>  <puerto>  <imagen>  <N−threads>  <N−ciclos>
void *socket_thread(void *args);

typedef struct clientArgs
{
    char* addrIP;
    char* imageName;
    int portNum;
    int nCycles;
} clientArgs;

int main(int argc, char *argv[]) {

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

    // socket params for each thread
    clientArgs *args = malloc(sizeof(clientArgs));
    args->addrIP = ip;
    args->imageName = imageName;
    args->portNum = port;
    args->nCycles = nCycles;
    int totalConection = nThreads*nCycles;

    //amount of needed threads
    pthread_t threads[totalConection];

    for(int i=0; i<totalConection; i++)
    {
        if(pthread_create(&threads[i], NULL, socket_thread, (void*) args) != 0){
            printf("Error with pthread_create");
            exit(1);
        }
    }
    for(int i=0; i<totalConection; i++)
    {
        if(pthread_join(threads[i], NULL) != 0)
        {
            perror("Could not join the thread");
            return 1;
        }
    }

    return 0;
}

void *socket_thread(void *args){
    clientArgs *cArgs = (clientArgs*) args;
    printf("args: %s %s %d %d \n", cArgs->addrIP, cArgs->imageName, cArgs->portNum, cArgs->nCycles);

    //Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    printf("sockfd %d \n", sockfd);

    // set server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;                           // using IPv4
    server_addr.sin_addr.s_addr = inet_addr(cArgs->addrIP);     // Replace with server IP address
    server_addr.sin_port = htons(cArgs->portNum);               // Replace with server port number

    // request connection to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Error connecting to server");
        exit(EXIT_FAILURE);
    }
    
    // Open the image file in binary mode
    FILE* fp = fopen(cArgs->imageName, "rb");
    if (fp == NULL) {
        printf("Error opening file\n");
        exit(EXIT_FAILURE);
    }

    // Check the amount of bytes that need to be sent
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    printf("file size in bytes: %ld \n", file_size);

    // Sending the image in chunks
    char buffer[CHUNCK_SIZE];
    int bytes_read, bytes_sent;
    while((bytes_read = fread(buffer, 1, CHUNCK_SIZE, fp)) > 0){
        bytes_sent = send(sockfd, buffer, bytes_read, 0);
        if ((bytes_sent < 0) || (bytes_sent != bytes_read)) {
            printf("Error sending data");
            exit(EXIT_FAILURE);
        }
    }

    // Cleaning up
    fclose(fp);
    close(sockfd);
}

