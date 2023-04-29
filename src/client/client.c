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
    int nThreads;
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
    args->nThreads = nThreads; 
    int totalConection = nThreads*nCycles;

    //Text del emisor
    FILE* TxtEmisor=fopen("LogFiles/EmisorLog.txt","a");
    
    //0 secuencial, 1 hilos, 2 heavy y 3 preheavy
    int tipoServidor;
    if(port == 8080){
        tipoServidor=0;
    }else if(port == 9000){ 
        tipoServidor=1;
    }else if(port == 5000){
        tipoServidor=2;
    }else{
        tipoServidor=3;
    }

    // Escribir la info en el log file, se escribe una linea al final del archivo
    char infoFormato[] = "ID: %d | S: %d | Num hilos: %d | Num ciclos: %d | Total: %d\n";
    fprintf(TxtEmisor, infoFormato, getpid(),tipoServidor, nThreads,nCycles, totalConection);
    
    //amount of needed threads
    pthread_t threads[nThreads];

    for(int i=0; i<nThreads; i++)
    {
        if(pthread_create(&threads[i], NULL, socket_thread, (void*) args) != 0){
            printf("Error with pthread_create");
            exit(1);
        }
    }
    for(int i=0; i<nThreads; i++)
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
    // send metadata for the charts. htonl -> Host to Network
    int metadata[4];
    metadata[0] = htonl(sockfd);                       // socket descriptor
    metadata[1] = htonl(cArgs->nThreads);              // number of threads
    metadata[2] = htonl(cArgs->nCycles);               // number of cycles
    metadata[3] = htonl(getpid());                     // number of cycles
    printf("--------\n [0]: %d \n [1]: %d \n [2]: %d [3]: %d \n--------\n", ntohl(metadata[0]), ntohl(metadata[1]), ntohl(metadata[2]),ntohl(metadata[3]));
    int bytesMD_sent = send(sockfd, &metadata, sizeof(metadata), 0);

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

    for(int i=0; i<cArgs->nCycles; i++){
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
    }
    // Cleaning up
    fclose(fp);
    close(sockfd);
}