#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <time.h>
#include "../../include/shared.h"

void handle_request(int socket_fd, int image_count);

int main(int argc, char const *argv[]){
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

    // Setting socket configs
    int optRes = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (optRes < 0) {
        perror("Error with setsockopt call");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the specified address and port
    int bindRes = bind(socket_fd, (struct sockaddr *)&address, sizeof(address));
    if (bindRes<0) {
        perror("Error with bind call");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    int queue_max = 50;
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
    //Validar la carpeta si ya posee 100
    DIR *dir;
    struct dirent *ent;
    int count = 0;

    //Text del secuencial
    FILE* TxtSecuencial=fopen("LogFiles/SecuencialLog.txt","a");
    
    while(1){
        printf("Waiting for a connection on port %d. IP: %s\n", PORT, inet_ntoa(sin.sin_addr));
        fflush(stdout);
        
        // Wait and accept incoming connections and handle them
        if ((new_socket = accept(socket_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        
        int count = 0;
        dir = opendir("TestImg");
        // Cuenta el número de archivos en el directorio
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) { // DT_REG es un archivo regular
                count++;
            }
        }
        // Cierra el directorio
        closedir(dir);


        // Imprime el número de archivos y determina si hay menos de 100
        printf("El número de archivos es: %d\n", count);
        if (count < 100) {
            //Tiempo de CPU 
            struct timespec startG1, endG1;
            clock_gettime(CLOCK_MONOTONIC, &startG1);
            handle_request(new_socket, count);
            clock_gettime(CLOCK_MONOTONIC, &endG1);
            double timeG1 = (endG1.tv_sec - startG1.tv_sec) +(endG1.tv_nsec - startG1.tv_nsec) / 1e9;
            
            // Escribir la info en el log file, se escribe una linea al final del archivo
            char infoFormato[] = "G1: %d     | %f             \n";
            fprintf(TxtSecuencial, infoFormato, new_socket, timeG1);
            fflush(TxtSecuencial);

        }close(new_socket);
        
    }
    close(socket_fd);
    return 0;
}

void handle_request(int socket_fd, int image_count)
{
    // Open a file to write the image data
    char imageName[25];
    sprintf(imageName, "TestImg/image%d.jpg", image_count);
    printf("image: %s \n", imageName);
    FILE* fp = fopen(imageName, "wb");

    char buffer[CHUNCK_SIZE];
    int bytes_received, bytes_written;
    // Receive the image data in chunks and write to file
    while ((bytes_received = recv(socket_fd, buffer, CHUNCK_SIZE, 0)) > 0) {
        bytes_written = fwrite(buffer, 1, bytes_received, fp);
        if (bytes_written != bytes_received) {
            printf("Error writing data\n");
            break;
        }
    }
}
