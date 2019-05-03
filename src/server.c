#include <stdio.h>
#include "../includes/StreamServer.h"

int main(){
    int s_fd;
    int err_check;
    struct addrinfo *server = NULL;
    setup_server(server, &s_fd);
    run_server(s_fd);

    return 0;
}
