#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define OK 200
#define NOT_FOUND 404
#define BAD_REQUEST 400

size_t size_of_file(FILE * file);
FILE * process_response(char ** file_name, int * status_code, size_t *s_file);
void write_log(char * buffer, char * c_addr);

#endif