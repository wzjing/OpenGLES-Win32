#include "win-util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELEMENT_SIZE 32

void load_file(char **content, char *filename) {
    printf("load_file\n");
    FILE *stream;
    if (fopen_s(&stream, filename, "r+t") == 0) {
        char buffer[ELEMENT_SIZE];
        while (feof(stream) != 0) {
            printf("Opening file: %s\n", filename);
            memset(buffer, '\0', ELEMENT_SIZE);
            size_t size = fread_s(buffer, ELEMENT_SIZE, ELEMENT_SIZE, 1, stream);

//            realloc(*content, sizeof(*content) + size);
            buffer[ELEMENT_SIZE] = '\0';
//            strcat_s(*content, ELEMENT_SIZE, buffer);
            printf("Read: %s\n", buffer);
        }
        fclose(stream);
    } else {
        printf("Unable to open file: %s\n", filename);
    }

}

