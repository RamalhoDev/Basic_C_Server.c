#ifndef STREAMSERVER_H
#define STREAMSERVER_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include "file.h"
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define PORT "8080"
#define CLIENTS 10
#define BUFFERSIZE 1024
#define OK 200
#define BAD_REQUEST 400
#define NOT_FOUND 404
#define HTTP_OK "HTTP/1.1 200 OK\n"
#define HTTP_NOT_FOUND "HTTP/1.1 404 Not Found\n"
#define HTTP_BAD_REQUEST "HTTP/1.1 400 Bad Request\n"

typedef struct{
    char * addr;
    int client_fd;
}tInfo;

int find_open_port(struct addrinfo * list, struct addrinfo * server);
void handle_clients(int s_fd);
void setup_server(struct addrinfo * server, int * s_fd);
void run_server(int s_fd);
char * process_buffer(char ** buffer);
void process_requests(tInfo *log);
char * headers(int status_code,char * type, size_t size_of_file);
char * create_response(int status_code, char * file_name, size_t size_of_file);
void * get_sockaddr(struct sockaddr * s);
#endif