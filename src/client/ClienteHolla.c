#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080

void *send_hello(void *args) {
    int sockfd, n;
    struct sockaddr_in serv_addr;
    char hello[] = "Hola";
    int num_iterations = ((int*)args)[0];
    int thread_id = ((int*)args)[1];

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

    // Enviar "Hola" N veces
    for (int i = 0; i < num_iterations; i++) {
        //hello[4]='L'; 
        if (send(sockfd, hello, strlen(hello), 0) < 0) {
            perror("Error al enviar mensaje al servidor");
            pthread_exit(NULL);
        }
        printf("Mensaje enviado desde hilo %d, iteración %d\n", thread_id, i+1);
    }

    close(sockfd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int num_threads = atoi(argv[1]);
    int num_iterations = atoi(argv[2]);

    pthread_t threads[num_threads];
    int thread_args[num_threads][2];

    for (int i = 0; i < num_threads; i++) {
        thread_args[i][0] = num_iterations;
        thread_args[i][1] = i + 1;
        pthread_create(&threads[i], NULL, send_hello, &thread_args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Todos los mensajes fueron enviados.\n");
    pthread_exit(NULL);
}
