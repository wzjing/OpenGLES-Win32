//
// Created by android1 on 4/19/2018.
//

#ifndef OPENGLES_NATIVE_LOG_H
#define OPENGLES_NATIVE_LOG_H

#include <stdio.h>

#define TAG "Native"
#define L_FORMAT "%-8s/%-2s: %s\n"
#define L_DEBUG "D"
#define L_INFO "I"
#define L_WARN "W"
#define L_ERROR "E"

#ifdef WIN32
#define __GUN_FORMAT__
#else
#define __GUN_FORMAT__ __attribute__(format(printf, 1, 2))
#endif


#define MAX_LOG_SIZE 2048

void logd(const char *message, ...) __GUN_FORMAT__;

void logi(const char *message, ...) __GUN_FORMAT__;

void logw(const char *message, ...) __GUN_FORMAT__;

void loge(const char *message, ...) __GUN_FORMAT__;

#endif //OPENGLES_NATIVE_LOG_H
