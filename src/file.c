#include "../includes/file.h"

size_t size_of_file(FILE * file){
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

FILE * process_response(char * file_name, int * status_code, size_t * s_file){
    char aux[64] = "./files/";
    strcat(aux, file_name);
    if(*status_code == BAD_REQUEST){
        FILE *file = fopen("./files/BAD_REQUEST.html", "rb");
        *s_file = size_of_file(file);
        *status_code = NOT_FOUND;
        return file;        
    }
    FILE *file = fopen(file_name, "rb");
    if(file != NULL){
        *s_file = size_of_file(file);
        *status_code = OK;
        return file;
    }else{
        file = fopen("./files/NOT_FOUND.html", "rb");
        *s_file = size_of_file(file);
        *status_code = NOT_FOUND;
        return file;
    }
}