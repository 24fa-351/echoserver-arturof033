#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

#define PORT 50001
#define LISTEN_BACKLOG 5
#define BUFFER_SIZE 1025

void handleConnection(int a_client){

    bool read_data = true;
    char buffer[BUFFER_SIZE];

   while (true){
        //char buffer[1024];
        memset(buffer, 0, sizeof buffer);
        int bytes_read = read(a_client, buffer, sizeof(buffer));
        if (!bytes_read) break;
        printf("Recieved: %s\n", buffer);
        write(a_client, buffer, bytes_read);

    }
}

int main(int argc, char* argv[]){

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in socket_address;
    memset(&socket_address, '\0', sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address.sin_port = htons(PORT);

    int returnval;

    //connect to network port 50001
    returnval = bind(
        socket_fd, (struct sockaddr*)&socket_address, sizeof(socket_address));


    returnval = listen(socket_fd, LISTEN_BACKLOG);

    struct sockaddr_in client_address;
    //socklen_t client_address_len = sizeof(client_address);

    // different client_fd for every "person" you're talking to
    bool read_data = true;

    while(read_data)
    {
        socklen_t client_address_len = sizeof(client_address);
        int client_fd = accept(
        socket_fd, (struct sockaddr*)&client_address, &client_address_len);
        
        if (client_fd < 0){
            read_data = false;
        }

        handleConnection(client_fd);
    }

    return 0;

}
