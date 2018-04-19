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
    printf("load_file\n");
    FILE *stream;
    if (fopen_s(&stream, filename, "r+t") == 0) {
        unsigned char buffer[BUFFER_SIZE] = "\0";
        *content = malloc(1);
        memset(*content, 0, 1);
        while (1) {
            // Reset and read
            logi("Debug1");
            memset(buffer, 0, BUFFER_SIZE);
            logi("Debug2");
            fread_s(buffer, BUFFER_SIZE, ELEMENT_SIZE, ELEMENT_COUNT, stream);
            logi("Debug3");
            // Append to result
            logi("Debug4");
//            buffer[BUFFER_SIZE] = '\0';
            logi("Debug5");
            logi("Buffer: \n%s", buffer);
            logd("content-%d, buffer-%d", strlen(*content), strlen(buffer));
            strcat_s(*content, strlen(*content) + strlen(buffer) + 1, buffer);
            logi("Debug6");
            if (feof(stream) != 0) {
                break;
            }
        }
        logi("finished");
        fclose(stream);
        logd("Read: \n--------%10s--------\n%s\n--------    end   --------", filename, *content);
    } else {
        printf("Unable to open file: %s\n", filename);
    }
}

int get_file_size(char *filename) {
    struct _stat info;
    _stat(filename, &info);
    return info.st_size;
}

