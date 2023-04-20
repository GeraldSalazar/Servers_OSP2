#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#define PORT 8080

void handle_request(int sockfd) {
  char buffer[1024] = {0};
  while(1){
    int valread = read(sockfd, buffer, 1024);
    if(valread == 0){
      printf("\nConexion cerrada \n");
      break;
    }
    fflush(stdout);
    printf("Mensaje int: %d\n", valread);
    printf("Mensaje recibido: %s\n", buffer);  
  }
  //Filtro
  close(sockfd);
}

int main() {
  int server_fd, new_socket, valread;
  struct sockaddr_in address;
  int opt = 1;
  int fifo[1024];
  int addrlen = sizeof(address);

  // Crear el socket del servidor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Error al crear el socket del servidor");
    exit(EXIT_FAILURE);
  }

  // Configurar opciones del socket
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("Error al configurar opciones del socket");
    exit(EXIT_FAILURE);
  }

  // Configurar la dirección del servidor
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Asociar el socket con la dirección del servidor
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Error al asociar el socket con la dirección del servidor");
    exit(EXIT_FAILURE);
  }

  // Escuchar por conexiones entrantes
  if (listen(server_fd, 3) < 0) {
    perror("Error al poner el socket en modo escucha");
    exit(EXIT_FAILURE);
  }
  // Esperar por conexiones entrantes y procesarlas
  while (1) {
    printf("Esperando por conexiones entrantes...\n");

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
      perror("Error al aceptar la conexión entrante \n");
      exit(EXIT_FAILURE);
    }
    //meterlo en el FIFO
    printf("\nConexión aceptada desde %d:%d\n", 1, ntohs(address.sin_port));
    fflush(stdout);
    handle_request(new_socket);
  }
  close(server_fd);
  return 0;
}