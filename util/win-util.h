//
// Created by android1 on 4/18/2018.
//

#ifndef OPENGLES_WIN_UTIL_H
#define OPENGLES_WIN_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif // cplusplus

void load_file(char **content, char *filename);

int get_file_size(char *filename);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif //OPENGLES_WIN_UTIL_H
