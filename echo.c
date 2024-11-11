#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>

#define DEFAULT_PORT 50001
#define LISTEN_BACKLOG 5
#define BUFFER_SIZE 1024

// incoming param was malloced elsewhere; is freed here
void* handleConnection(void* a_client_ptr){

    int a_client = *(int*)a_client_ptr;
    free(a_client_ptr);

    bool read_data = true;

    while (read_data){
        char buffer[1024];
        int bytes_read = read(a_client, buffer, sizeof(buffer));
        if (bytes_read == 0){
            printf("end of connection on socket_fd %d\n", a_client);
            close(a_client);
            read_data = false;
        }

        printf("Recieved from socket_fd %d: %s\n", a_client, buffer);
        write(a_client, buffer, bytes_read);

    }

   return NULL;
}

int main(int argc, char* argv[]){

    int port = DEFAULT_PORT;

    if(strcmp(argv[1], "-p") == 0){
        port = atoi(argv[2]);
    }

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in socket_address;
    memset(&socket_address, '\0', sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address.sin_port = htons(port);

    int returnval;

    //connect to network port 50001
    returnval = bind(socket_fd, (struct sockaddr*)&socket_address, 
                     sizeof(socket_address));
    if (returnval == -1){
        perror("bind");
        return 1;
    }

    returnval = listen(socket_fd, LISTEN_BACKLOG);

    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    // different client_fd for every "person" you're talking to
    bool read_data = true;

    while(read_data)
    {

        int client_fd = accept(
            socket_fd, (struct sockaddr*)&client_address, &client_address_len);
        if (client_fd == -1){
            perror("accept");
            return 1;
        }

        int* client_fd_ptr = (int*)malloc(sizeof(int));
        *client_fd_ptr = client_fd;

        pthread_t thread;
        pthread_create(&thread, NULL, handleConnection, (void*)client_fd_ptr);

    }

    return 0;

}
