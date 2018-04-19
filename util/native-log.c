#include "native-log.h"
#include <stdarg.h>

void logd(const char *message, ...) {
    va_list args;
    char buffer[MAX_LOG_SIZE];
            va_start(args, message);
    vsprintf_s(buffer, MAX_LOG_SIZE * sizeof(char), message, args);
            va_end(args);
    printf(L_FORMAT, TAG, L_DEBUG, buffer);
}

void logi(const char *message, ...) {
    va_list args;
    char buffer[MAX_LOG_SIZE];
            va_start(args, message);
    vsprintf_s(buffer, MAX_LOG_SIZE * sizeof(char), message, args);
            va_end(args);
    printf(L_FORMAT, TAG, L_INFO, buffer);
}

void logw(const char *message, ...) {
    va_list args;
    char buffer[MAX_LOG_SIZE];
            va_start(args, message);
    vsprintf_s(buffer, MAX_LOG_SIZE * sizeof(char), message, args);
            va_end(args);
    printf(L_FORMAT, TAG, L_WARN, buffer);
}

void loge(const char *message, ...) {
    va_list args;
    char buffer[MAX_LOG_SIZE];
            va_start(args, message);
    vsprintf_s(buffer, MAX_LOG_SIZE * sizeof(char), message, args);
            va_end(args);
    printf(L_FORMAT, TAG, L_ERROR, buffer);
}
