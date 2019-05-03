#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#define PORT "8080"
#define CLIENTS 10
#define BUFFERSIZE 513

int main(int argc,char * argv[]){
    char buffer[BUFFERSIZE];
    int s_fd;
    struct addrinfo aux, *l_server, *server;
    socklen_t size;
    memset(&aux, 0, sizeof(aux));
    aux.ai_family = AF_UNSPEC;
    aux.ai_socktype = SOCK_STREAM;
    aux.ai_flags = AI_PASSIVE;
    if(getaddrinfo(argv[1], PORT, &aux, &l_server) == -1){
        puts("ERROR: Couldn't get info");
        exit(1);
    }

    for(server = l_server; server != NULL; server = server->ai_next){
        s_fd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
        if(s_fd == -1){
            puts("Couldn't create socket. Continuing...");
            continue;
        }

        if(connect(s_fd, server->ai_addr, server->ai_addrlen) == -1){
            close(s_fd);
            puts("Couldn't connect to server");
            continue;
        }
        break;
    }
    freeaddrinfo(l_server);

    if(server == NULL){
        puts("ERROR: Client failed to connect");
        exit(1);
    }
    send(s_fd, "GET /coco.txt HTTP/1.1", 30,  0);

    recv(s_fd, buffer, BUFFERSIZE-1, 0);
    if(buffer != NULL){
        puts(buffer);
    }


    close(s_fd);
    return 0;
}