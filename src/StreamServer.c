#include "../includes/StreamServer.h"

char * headers(int status_code, char * file_name, size_t size_of_file){
    char * response = malloc(sizeof(char) * BUFFERSIZE/2);
    response[0] = '\0';
    char s_file[32];
    char * cont_type = "Content-Type: ";
    char * cont_len = "Content-Length: ";
    
    strcat(response, cont_len);
    snprintf(s_file, 32, "%lu\n", size_of_file);
    strcat(response, s_file);
    strcat(response, cont_type);
    if(status_code == BAD_REQUEST || status_code == NOT_FOUND){
            strcat(response, "text/");
            strcat(response, "html");
    }else{
        const char p = '.';
        char * aux = strchr(file_name, p);
        char * type;
        memcpy(type, &aux[1], strlen(aux));
        puts(type);
        if(!strcmp(type, "png") || !strcmp(type, "jpg") || !strcmp(type, "jpeg"))
            strcat(response, "image/");    
        else
            strcat(response, "text/");
        strcat(response, type);
    }
    strcat(response, "\nAccept-Range: bytes");
    strcat(response, "\nConnection: closed");
    strcat(response, "\n\n");
    return response;
}

char * create_response(int status_code, char * file_name, size_t size_of_file){
    char * response = malloc(sizeof(char) * BUFFERSIZE);
    response[0]= '\0';
    char * header;
    switch(status_code){
        case  OK:
            strcat(response, HTTP_OK);
            header = headers(status_code,file_name, size_of_file);
            strcat(response, header);
            break;
        case BAD_REQUEST:
            strcat(response ,HTTP_BAD_REQUEST);
            header = headers(status_code,file_name, size_of_file);
            strcat(response, header);
            break;
        case NOT_FOUND:
            strcat(response, HTTP_NOT_FOUND);
            header = headers(status_code,file_name, size_of_file);
            strcat(response, header);
            break;
    }
    free(header);
    return response;
}

char * process_buffer(char ** buffer){
    char * arquivo = malloc(sizeof(char) * 64);
    char aux = '/';
    char * aux3 = strchr(buffer, aux);

    for(int i = 1; i < strlen(aux3); i++){
        if(aux3[i] == ' ')
            break;
        arquivo[i-1] = aux3[i];
    }
    // arquivo = strtok_r(aux3, " ", &aux3);
    return arquivo;
}

int find_open_port(struct addrinfo * list, struct addrinfo * server){
    int socketfd;
    int yes = 1;    
    for(server = list; server != NULL; server = list->ai_next){
        socketfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
        if(socketfd == -1){
            continue;
        }

        if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
            exit(1);
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
    FILE * file;
    int c_fd;

    while(1){
        char *file_name = NULL, *response = NULL;
        int status_code = 0;
        size_t size_of_file = 0;
        char buffer[BUFFERSIZE];

        c_fd = accept(s_fd, (struct sockaddr *) &client, &c_size);
        if( c_fd == -1){
            puts("ERROR: Connection to client failed");
            continue;
        }
        if(recv(c_fd, buffer, BUFFERSIZE-1, 0) == -1){
            puts("ERROR: Couldn't receive package");
        }
        file_name = process_buffer(&buffer);
        file = process_response(file_name, &status_code, &size_of_file);
        char * file_content = malloc(sizeof(char) * (size_of_file+10));
    
        response = create_response(status_code, file_name ,size_of_file);
        if(send(c_fd, response, strlen(response)+1, 0) == -1){
            puts("ERROR: Couldn't send response");
        }
        fread(file_content, 1, size_of_file, file);
        
        if(send(c_fd, file_content, size_of_file, 0) == -1){
            puts("ERROR: Couldn't send response");
        }

        close(c_fd);
        fclose(file);
        free(file_content);
        free(response);
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
    }//
    
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