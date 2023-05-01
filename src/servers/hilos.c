#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>
#include "../../include/shared.h"
#include "../../Native_Sobel/src/Sobel.h"

typedef struct {
    int new_socket;
    int num;
    FILE* Txt;
} structParametros;

void *handle_request(void *parametro);

int main(int argc, char const *argv[]){
    int socket_fd, new_socket;
    int opt = 1;
    char buffer[1024] = {0};

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;               //represents the address family for IPv4
    address.sin_addr.s_addr = inet_addr(IPH);              //any IP address on the local machine.
    address.sin_port = htons( PORTH );           //takes a 16-bit integer value in host byte order as its argument, and returns the corresponding value in network byte order.

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
    //struc para los parametros
    structParametros misParam;

    //Text del hilos
    FILE* TxtHilos=fopen("LogFiles/HilosLog.txt","a");
    misParam.Txt=TxtHilos;
    
    while(1){
        printf("\nWaiting for a connection on port %d. IP: %s\n", PORTH, inet_ntoa(sin.sin_addr));
        fflush(stdout);
        // Wait and accept incoming connections and handle them
        if ((new_socket = accept(socket_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        
        int count = 0;
        dir = opendir("filtered");
        // Cuenta el número de archivos en el directorio
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) { // DT_REG es un archivo regular
                count++;
            }
        }
        // Cierra el directorio
        closedir(dir);

        // Imprime el número de archivos y determina si hay menos de 100
        printf("\nEl número de archivos es: %d\n", count);
        if (count < 100) {
            
            misParam.new_socket=new_socket;
            misParam.num=count;

            //Creamos el nuevo hilo
            pthread_t thread_id; 
            pthread_create(&thread_id, NULL, handle_request, (void *) &misParam);
            pthread_join(thread_id, NULL);

        }
        close(new_socket);
        
    }
    close(socket_fd);
    return 0;
}

void *handle_request(void *parametro)
{
    structParametros *misParametro = (structParametros *) parametro;
    
    //Tiempo inicial del CPU 
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // First, read the metadata from client
    int metadata[4];
    recv(misParametro->new_socket, &metadata, sizeof(metadata), 0);
    int socketD = ntohl(metadata[0]);
    int nThreads = ntohl(metadata[1]);
    int nCycles = ntohl(metadata[2]);
    int idProcess= ntohl(metadata[3]);

    printf("--------\n [0]: %d \n [1]: %d \n [2]: %d [3]: %d \n--------\n", socketD, nThreads, nCycles,idProcess);
    
    // Open a file to write the image data
    char imageName[40];
    
    sprintf(imageName, "TestImg/image_%d.jpg", misParametro->num);
    printf("image: %s \n", imageName);
    FILE* fp = fopen(imageName, "wb");
        
    char buffer[CHUNCK_SIZE];
    int bytes_received, bytes_written;


    //N-Ciclos
    for(int i = 0; i < nCycles; i++){
        if(misParametro->num < 100){
            while ((bytes_received = recv(misParametro->new_socket, buffer, CHUNCK_SIZE, 0)) > 0) {
                bytes_written = fwrite(buffer, 1, bytes_received, fp);
                if (bytes_written != bytes_received) {
                    printf("Error writing data\n");
                    break;
                }
            }
            sobel(imageName, misParametro->num);
            misParametro->num++;
        }
    }
    
    fclose(fp);
    //Tiempo final
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time = (end.tv_sec - start.tv_sec) +(end.tv_nsec - start.tv_nsec) / 1e9;
    
    //Toma de la memoria
    char bufferTest[50];
    char command[30];
    sprintf(command, "ps -p %d -o rss", getpid());
    FILE* fpa = popen(command, "r");
    if (fpa == NULL) {
        printf("Failed to execute command\n" );
        exit(1);
    }
    fread(bufferTest, sizeof(bufferTest), 30, fpa);

    size_t newline_pos = strcspn(bufferTest, "\n"); // Find position of newline
    if (newline_pos < strlen(bufferTest)) { // If newline found
        bufferTest[newline_pos] = ' '; // Overwrite with null terminator
    }

    //Memoria final
    int rss;
    if (sscanf(bufferTest, "   RSS  %d", &rss) != 1) {
        printf("Failed to parse output of command\n");
        exit(1);
    }
    printf("Output of ls command: %d\n", rss);
    pclose(fpa);

    // Escribir la info en el log file, se escribe una linea al final del archivo
    char infoFormato[] = "%f,%d,%d,%d\n";
    fprintf(misParametro->Txt, infoFormato,time,idProcess,socketD,rss);
    fflush(misParametro->Txt);
}
