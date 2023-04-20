#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080

void handle_request(int sockfd) {
    char buffer[1024] = {0};
    int valread = read(sockfd, buffer, 1024);
    printf("Mensaje recibido: %s\n", buffer);
    send(sockfd, buffer, strlen(buffer), 0);
}

int main(int argc, char *argv[]) {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Crear socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Especificar dirección y puerto para el socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Asociar socket a la dirección y puerto especificados
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Error al asociar el socket a la dirección y puerto");
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_fd, 3) < 0) {
        perror("Error al escuchar conexiones entrantes");
        exit(EXIT_FAILURE);
    }

    // Esperar por conexiones entrantes y manejarlas en procesos hijos
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Error al aceptar la conexión entrante");
            exit(EXIT_FAILURE);
        }

        // Crear proceso hijo para atender la solicitud
        pid_t pid = fork();

        if (pid < 0) {
            perror("Error al crear proceso hijo");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Proceso hijo
            close(server_fd); // El proceso hijo no necesita el socket del servidor
            handle_request(new_socket);
            close(new_socket);
            exit(EXIT_SUCCESS);
        } else {
            // Proceso padre
            close(new_socket); // El proceso padre no necesita el socket del cliente
        }
    }

    return 0;
}
