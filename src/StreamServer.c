#include "../includes/StreamServer.h"

int find_open_port(struct addrinfo * list, struct addrinfo * server){
    int socketfd;
    for(server = list; server != NULL; server = list->ai_next){
        socketfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
        if(socketfd == -1){
            continue;
        }

        if(bind(socketfd, server->ai_addr, server->ai_addrlen) == -1){
            close(socketfd);
            continue;
        }
        break;
    }
    freeaddrinfo(list);
    return socketfd;
}

void handle_clients(int s_fd){
    struct sockaddr_storage client;
    socklen_t c_size = sizeof(client);
    char buffer[BUFFERSIZE];
    while(1){
        int c_fd = accept(s_fd, (struct sockaddr *) &client, &c_size);
        if( c_fd == -1){
            puts("ERROR: Connection to client failed");
            continue;
        }

        if(recv(c_fd, buffer, BUFFERSIZE-1, 0) == -1){
            puts("ERROR: Couldn't receive package");
        }   

        puts(buffer);
        
        if(send(c_fd, "ISSO AE PORRA", 13, 0) == -1){
            puts("ERROR: Couldn't send response");
        }
        close(c_fd);
    }
}

void setup_server(struct addrinfo * server, int * s_fd){
    struct addrinfo aux, *l_server;
    
    memset(&aux, 0, sizeof(aux));
    aux.ai_family = AF_UNSPEC;
    aux.ai_socktype = SOCK_STREAM;
    aux.ai_flags = AI_PASSIVE;

    if(getaddrinfo(NULL, PORT, &aux, &l_server) != 0){
        puts("ERROR: Couldn't get info.");
        exit(1);
    }
    
    *s_fd = find_open_port(l_server, server);
}

void run_server(int s_fd){
    int err_check = listen(s_fd, CLIENTS);
    if(err_check == -1){
        puts("ERROR: Server not listening.");
        exit(2);
    }
    handle_clients(s_fd);
    close(s_fd);
}