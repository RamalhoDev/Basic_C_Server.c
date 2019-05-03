#ifndef STREAMSERVER_H
#define STREAMSERVER_H
#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#define PORT "8080"
#define CLIENTS 10
#define BUFFERSIZE 513

int find_open_port(struct addrinfo * list, struct addrinfo * server);
void handle_clients(int s_fd);
void setup_server(struct addrinfo * server, int * s_fd);
void run_server(int s_fd);

#endif