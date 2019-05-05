#include "../includes/file.h"

size_t size_of_file(FILE * file){
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}



FILE * process_response(char ** file_name, int * status_code, size_t * s_file){
    char aux[64] = "\0";
    if(*status_code == BAD_REQUEST){
        char n_f[30] = "BAD_REQUEST.html";
        memcpy(*file_name, n_f, 64);    
        FILE *file = fopen("./files/BAD_REQUEST.html", "rb");
        *s_file = size_of_file(file);
        *status_code = BAD_REQUEST;
        return file;        
    }

    if(*(file_name[0]) != '\0'){
        strcat(aux,"./files/");
        strcat(aux, *file_name);
    }

    FILE *file = fopen(aux, "rb");

    if(file == NULL){
        char * n_f = "NOT_FOUND.html";
        file = fopen("./files/NOT_FOUND.html", "rb");
        memcpy(*file_name, n_f, 64);    
        *s_file = size_of_file(file );
        *status_code = NOT_FOUND;
    }else if(file != NULL){
        *s_file = size_of_file(file);
        *status_code = OK;
    }

    return file;
}

void write_log(char * buffer, char * c_addr){
    FILE * LOG = fopen("./log.txt", "a+");
    fprintf(LOG, "IP: %s\n", c_addr);
    fprintf(LOG, "%s", buffer);
    fclose(LOG);
}