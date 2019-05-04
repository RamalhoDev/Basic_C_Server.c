#include "../includes/StreamServer.h"

char * get_time(){
    time_t res_time;
    time(&res_time);
    struct tm *info = gmtime(&res_time);;
    char * asct = asctime(info);
    char * response = malloc(sizeof(char)*64);
    memset(response, 0, 64);
    asct[strlen(asct)-1] = '\0';
    
    char * week_day = strtok_r(asct, " ", &asct);
    char * month = strtok_r(NULL, " ", &asct);
    char * day = strtok_r(NULL, " ", &asct);
    char * s_time = strtok_r(NULL, " ", &asct);
    char * year = strtok_r(NULL, " ", &asct);

    snprintf(response, 63, "Date: %s, %s %s %s %s GMT", week_day, day, month, year, s_time);
    
    return response;
}

char * headers(int status_code, char * file_name, size_t size_of_file){
    int size = BUFFERSIZE/2;
    char * response = malloc(sizeof(char) * size), s_file[32];
    char * t = get_time();
    char * last = "\nAccept-Range: bytes\nConnection: closed\n\n";
    response[0] = '\0';
    if(status_code == BAD_REQUEST || status_code == NOT_FOUND){
        size -= strlen("230\nContent-Type: text/html");
        strncat(response, "230\nContent-Type: text/html", size);
    }else{
        char content_type[64];
        char * aux = strchr(file_name, '.');
        char * type = malloc(sizeof(char) * 10);
        memcpy(type, &aux[1], strlen(aux));
        snprintf(content_type, size, "%lu\nContent_Type: ", size_of_file);
        size-=strlen(content_type);
        strncat(response, content_type, size-1);

        if(!strcmp(type, "png") || !strcmp(type, "jpg") || !strcmp(type, "jpeg")){
            size -=strlen("image/");
            strncat(response, "image/", size-1);    
        }else if(!strcmp(type, "html") || !strcmp(type, "txt")){
            size-=strlen("image/");
            strncat(response, "text/", size - strlen("text/")-1);
        }
        size-=strlen(type);
        strncat(response, type, size - 1);
        free(type);
    }
    size -= strlen(last);
    strncat(response, last, size);
 
    return response;
}

char * response_content(int * status_code, char **file_name){
    size_t size = 64;
    char * response = malloc(sizeof(char)* size);
    response = "\0";

    if(*status_code == BAD_REQUEST || *status_code == NOT_FOUND){
        strncat(response, "230\nContent-Type: text/html", size);
    }else{
        char content_type[64];
        char * aux = strchr(file_name, '.');
        char * type = malloc(sizeof(char) * 10);
        memcpy(type, &aux[1], strlen(aux));
        snprintf(content_type, size, "%lu\nContent_Type: ", size_of_file);
        size-=strlen(content_type);
        strncat(response, content_type, size-1);

        if(!strcmp(type, "png") || !strcmp(type, "jpg") || !strcmp(type, "jpeg")){
            size -=strlen("image/");
            strncat(response, "image/", size-1);    
        }else if(!strcmp(type, "html") || !strcmp(type, "txt")){
            size-=strlen("image/");
            strncat(response, "text/", size - strlen("text/")-1);
        }
        size-=strlen(type);
        strncat(response, type, size - 1);
        free(type);
    }
}

char * response_status(int * status_code){
    switch(*status_code){
        case  OK:
            return HTTP_OK;
        case BAD_REQUEST:
            return BAD_REQUEST;
        case NOT_FOUND:
            return NOT_FOUND;
    }
}

char * create_response(int status_code, char * file_name, size_t size_of_file){
    char * response = malloc(sizeof(char) * BUFFERSIZE);
    response[0]= '\0';
    char * http_status = response_status;
    char * date = get_time();
    header = headers(status_code,file_name, size_of_file);
    snprintf(response, BUFFERSIZE, "%s%s", HTTP_OK, header);
    header = headers(status_code,file_name, size_of_file);
    snprintf(response, BUFFERSIZE, "%s%s", HTTP_BAD_REQUEST, header);
    header = headers(status_code,file_name, size_of_file);
    snprintf(response, BUFFERSIZE, "%s%s", HTTP_NOT_FOUND, header);

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
    char *file_name = NULL;
    char *response = NULL;
    char buffer[BUFFERSIZE] = "\0";
    int status_code = 0;
    size_t size_of_file = 0;
    sem_t sm;
    sem_init(&sm, 0, 1);

    if(recv(info->client_fd, buffer, BUFFERSIZE-1, 0) == -1){
        puts("ERROR: Couldn't receive package");
    } 

    if(!(buffer[0]=='\0')){
        if(buffer[0] != 'G' ){
            status_code = BAD_REQUEST;
        }
        else
            file_name = process_buffer(&buffer);
        file = process_response(file_name, &status_code, &size_of_file);
        char * file_content = malloc(sizeof(char) * (size_of_file+1));

        response = create_response(status_code, file_name ,size_of_file);
        puts(response);
        if(send(info->client_fd, response, strlen(response), 0) == -1){
            puts("ERROR: Couldn't send response");
        }
        fread(file_content, 1, size_of_file, file);
        if(send(info->client_fd, file_content, size_of_file, 0) == -1){
            puts("ERROR: Couldn't send response");
        }

        sem_wait(&sm);
        write_log(buffer, info->addr);
        sem_post(&sm);

        close(info->client_fd);
        fclose(file);
        free(file_content);
        free(response);
        free(file_name);
        sem_destroy(&sm);
    }
    close(info->client_fd);
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