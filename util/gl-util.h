#ifndef GLUTIL_H
#define GLUTIL_H

#ifdef __cplusplus
extern "C" {
#endif // cplusplus

#ifdef WIN32
#define ESUTIL_API __cdecl
#define ESCALLBACK __cdecl
#else
#define ESUTIL_API
#define ESCALLBACK
#endif // WIN32

#define ES_WINDOW_RGB 0
#define ES_WINDOW_ALPHA 1
#define ES_WINDOW_DEPTH 2
#define ES_WINDOW_STENCIL 4
#define ES_WINDOW_MULTISAMPLE 8

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <stdio.h>
#include <stdlib.h>

#define LOGV(format, ...) printf(format, ## __VA_ARGS__)
#define LOGD(format, ...) printf(format, ## __VA_ARGS__)
#define LOGI(format, ...) printf(format, ## __VA_ARGS__)
#define LOGW(format, ...) printf(format, ## __VA_ARGS__)
#define LOGE(format, ...) printf(format, ## __VA_ARGS__)

typedef struct {
    GLfloat m[4][4];
} ESMatrix;

typedef struct ESContext ESContext;

struct ESContext {
    void *platformData;
    void *userData;
    GLint width;
    GLint height;

    EGLNativeDisplayType eglNativeDislpay;

    EGLNativeWindowType eglNativeWindow;

    EGLDisplay eglDisplay;

    EGLContext eglContext;

    EGLSurface eglSurface;

    void (ESCALLBACK *drawFunc)(ESContext *);

    void (ESCALLBACK *shutdownFunc)(ESContext *);

    void (ESCALLBACK *keyFunc)(ESContext *, unsigned char, int, int);

    void (ESCALLBACK *updateFunc)(ESContext *, float deltaTime);
};

/** Emulator Functions **/
LRESULT WINAPI ESWindowProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

GLboolean WinCreate(ESContext *esContext, const char *title);

GLboolean WinCreate(ESContext *esContext, const char *title);

void WinLoop ( ESContext *esContext );

/** OpenGL ES Functions **/

GLboolean ESUTIL_API esCreateWindow(ESContext *esContext, const char *title, GLint width, GLint height, GLuint flags);

void ESUTIL_API esRegisterDrawFunc(ESContext *esContext, void (ESCALLBACK *drawFunc)(ESContext *));

void ESUTIL_API esRegisterShutdownFunc(ESContext *esContext, void (ESCALLBACK *shutdownFunc)(ESContext *));

void ESUTIL_API esRegisterUpdateFunc(ESContext *esContext, void (ESCALLBACK *updateFunc)(ESContext *, float));

void
ESUTIL_API esRegisterKeyFunc(ESContext *esContext, void (ESCALLBACK *keyFunc)(ESContext *, unsigned char, int, int));

char *ESUTIL_API esLoadTGA(void *ioContext, const char * fileName, int *width, int *height);

void ESUTIL_API esLogMessage(const char *formatStr, ...);

//load shader by shader code
GLuint ESUTIL_API esLoadShader(GLenum shaderType, const char *pSource);

//create OpenGL ES program by [Vertex Shader code] and [Fragment Shader code]
GLuint ESUTIL_API esCreateProgram(const char *pVertexSource, const char *pFragmentSource);


//Check OpenGL ES error
void ESUTIL_API checkGlError(const char *op);

//print OpenGL ES String
void ESUTIL_API printGlInfo();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //GLUTIL_H