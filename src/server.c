#include <stdio.h>
#include <stdlib.h>
#include "../includes/StreamServer.h"

int main(){
    int s_fd;
    int err_check;
    struct addrinfo *server;
    setup_server(server, &s_fd);
    if(server == NULL){
        puts("ERROR: Server not initialized");
        return -3;
    }
    run_server(s_fd);

    return 0;
}
