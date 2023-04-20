#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080

void *send_image(void *thread_id) {
  int sockfd, n;
  struct sockaddr_in serv_addr;
  char *filename = "assets/sample_4_640x960.jpg";
  char buffer[1024] = {0};
  int num_iterations = *(int*)thread_id;

  // Crear el socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Error al crear el socket");
    pthread_exit(NULL);
  }

  // Especificar la dirección del servidor
  memset(&serv_addr, '0', sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    perror("Error al especificar la dirección del servidor");
    pthread_exit(NULL);
  }

  // Conectar al servidor
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Error al conectar al servidor");
    pthread_exit(NULL);
  }

  // Enviar la imagen N veces
  for (int i = 0; i < num_iterations; i++) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
      perror("Error al abrir el archivo");
      pthread_exit(NULL);
    }
    while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
      if (send(sockfd, buffer, n, 0) < 0) {
        perror("Error al enviar imagen al servidor");
        pthread_exit(NULL);
      }
    }
    fclose(fp);
    printf("Imagen enviada desde thread %d, iteración %d\n", (int)pthread_self(), i+1);
    usleep(100000); // Esperar 0.1 segundos entre envíos
  }

  close(sockfd);
  pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
  int num_threads = atoi(argv[1]);
  int num_iterations = atoi(argv[2]);
  pthread_t threads[num_threads];
  int thread_args[num_threads];
  int i;

  for (i = 0; i < num_threads; i++) {
    thread_args[i] = num_iterations;
    pthread_create(&threads[i], NULL, send_image, &thread_args[i]);
  }

  for (i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("Todos los mensajes fueron enviados.\n");
  pthread_exit(NULL);
}
