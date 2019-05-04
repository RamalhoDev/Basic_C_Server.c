#include "../includes/StreamServer.h"

char * get_time(){
    time_t res_time;
    struct tm *info;
    time(&res_time);
    info = gmtime(&res_time);
    char * asct = asctime(info);
    asct[strlen(asct)-1] = '\0';
    char * week_day = strtok_r(asct, " ", &asct);
    char * month = strtok_r(NULL, " ", &asct);
    char * day = strtok_r(NULL, " ", &asct);
    char * s_time = strtok_r(NULL, " ", &asct);
    char * year = strtok_r(NULL, " ", &asct);
    char * response = malloc(sizeof(char)*64);
    strcat(response, week_day);
    strcat(response, ", ");
    strcat(response, day);
    strcat(response, " ");
    strcat(response, month);
    strcat(response, " ");
    strcat(response, year);
    strcat(response, " ");
    strcat(response, s_time);
    strcat(response, " ");
    strcat(response, "GMT\n");
    
    return response;
}

char * headers(int status_code, char * file_name, size_t size_of_file){
    char * response = malloc(sizeof(char) * BUFFERSIZE/2), s_file[32];
    char * cont_type = "Content-Type: ", *cont_len = "Content-Length: ";
    char * t = get_time();
    response[0] = '\0';
    strcat(response, "Date: ");
    strcat(response, t);
    strcat(response, "Server: rcrServer/1.0\n");
    strcat(response, cont_len);
    if(status_code == BAD_REQUEST || status_code == NOT_FOUND){
        snprintf(s_file, 32, "%lu\n", 320);
        strcat(response, s_file);
        strcat(response, cont_type);
            strcat(response, "text/");
            strcat(response, "html");
    }else{
        snprintf(s_file, 32, "%lu\n", size_of_file);
        strcat(response, s_file);
        const char p = '.';
        char * aux = strchr(file_name, p);
        char * type = malloc(sizeof(char) * 10);
        memcpy(type, &aux[1], strlen(aux));
        if(!strcmp(type, "png") || !strcmp(type, "jpg") || !strcmp(type, "jpeg"))
            strcat(response, "image/");    
        else 
            if(type != NULL)
                strcat(response, "text/");
        strcat(response, type);
        free(type);
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
    memset(arquivo, 0, 64);
    char aux = '/';
    char * aux3 = NULL;
    aux3 = strchr(buffer, aux);
    for(int i = 1; i < strlen(aux3); i++){
        if(aux3[i] == ' '){
            break;
        }
        arquivo[i-1] = aux3[i];
    }
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

void * get_sockaddr(struct sockaddr * s){
    if(s->sa_family == AF_INET)
        return &(((struct sockaddr_in *)s)->sin_addr);
    else
        return &(((struct sockaddr_in6 *)s)->sin6_addr);
}

void process_requests(tInfo *info){
        FILE * file;
        char *file_name = NULL, *response = NULL;
        char buffer[BUFFERSIZE] = '\0';
        int status_code = 0;
        size_t size_of_file = 0;
        sem_t sm;
        sem_init(&sm, 0, 1);

        if(recv(info->client_fd, buffer, BUFFERSIZE-1, 0) == -1){
            puts("ERROR: Couldn't receive package");
        }
        if(buffer[0] != 'G')
            status_code = BAD_REQUEST;
        else
            file_name = process_buffer(&buffer);
        file = process_response(file_name, &status_code, &size_of_file);
        char * file_content = malloc(sizeof(char) * (size_of_file+1));

        sem_wait(&sm);
        write_log(buffer, info->addr);
        sem_post(&sm);

        response = create_response(status_code, file_name ,size_of_file);
        if(send(info->client_fd, response, strlen(response)+1, 0) == -1){
            puts("ERROR: Couldn't send response");
        }
        fread(file_content, 1, size_of_file, file);
        if(send(info->client_fd, file_content, size_of_file, 0) == -1){
            puts("ERROR: Couldn't send response");
        }

        close(info->client_fd);
        fclose(file);
        free(file_content);
        free(response);
        free(file_name);
        sem_destroy(&sm);
}

void handle_clients(int s_fd){
    pthread_t clients[CLIENTS];
    struct sockaddr_storage client;
    socklen_t c_size = sizeof(client);
    int c_fd, count = 0, err_check;
    char client_addr[INET6_ADDRSTRLEN];
    tInfo info;
    
    while(1){
        c_fd = accept(s_fd, (struct sockaddr *) &client, &c_size);
        if( c_fd == -1){
            puts("ERROR: Connection to client failed");
            continue;
        }

        inet_ntop(client.ss_family, get_sockaddr((struct sockaddr *)&client) ,client_addr, sizeof(client_addr));
        
        info.client_fd = c_fd;
        info.addr = client_addr;

        err_check = pthread_create(&clients[count], NULL, process_requests, &info);
        if(err_check){
            exit(1);
        }
        count++;
        if(count == 10){
            break;
        }
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