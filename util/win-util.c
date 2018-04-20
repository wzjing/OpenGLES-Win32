#include "win-util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "native-log.h"
#include <sys/stat.h>

#define ELEMENT_SIZE 1
#define ELEMENT_COUNT 256
#define BUFFER_SIZE (ELEMENT_COUNT * ELEMENT_SIZE +1)

void load_file(char **content, char *filename) {
    FILE *stream;
    if (fopen_s(&stream, filename, "r+t") == 0) {
        char buffer[BUFFER_SIZE] = "\0";
        size_t content_size = BUFFER_SIZE;
        *content = malloc(content_size);
        memset(*content, 0, 1);
        while (1) {
            // Reset and read
            memset(buffer, 0, BUFFER_SIZE);
            fread_s(buffer, BUFFER_SIZE, ELEMENT_SIZE, ELEMENT_COUNT, stream);

            // Append to result
//            logi("Buffer: \n%s", buffer);
            *content = realloc(*content, content_size += BUFFER_SIZE);
            strcat_s(*content, content_size, buffer);
            fflush(stream);
            if (feof(stream) != 0) {
                break;
            }
        }
        fclose(stream);
//        logd("Read: \n--------%10s--------\n%s\n--------    end   --------", filename, *content);
    } else {
        printf("Unable to open file: %s\n", filename);
    }
}

int get_file_size(char *filename) {
    struct _stat info;
    _stat(filename, &info);
    return info.st_size;
}

