#include "gl-util.h"
#include <windows.h>

#ifdef _WIN64
#define GWL_USERDATA GWLP_USERDATA
#endif

typedef struct {
    unsigned char IdSize, MapTye, ImageType;
    unsigned short PaletteStart, PaletteSize;
    unsigned char PaletteEntryDepth;
    unsigned short X, Y, Width, Height;
    unsigned char ColorDepth, Descriptor;
} TGA_HEADER;

typedef FILE esFile;


/** Platform Functions **/

LRESULT WINAPI ESWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT lRet = 1;

    switch (uMsg) {
        case WM_CREATE:
            break;

        case WM_PAINT: {
            ESContext *esContext = (ESContext *) (LONG_PTR) GetWindowLongPtr(hWnd, GWL_USERDATA);

            if (esContext && esContext->drawFunc) {
                esContext->drawFunc(esContext);
                eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
            }


            ValidateRect(esContext->eglNativeWindow, NULL);
        }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_CHAR: {
            POINT point;
            ESContext *esContext = (ESContext *) (LONG_PTR) GetWindowLongPtr(hWnd, GWL_USERDATA);

            GetCursorPos(&point);

            if (esContext && esContext->keyFunc)
                esContext->keyFunc(esContext, (unsigned char) wParam,
                                   (int) point.x, (int) point.y);
        }
            break;

        default:
            lRet = DefWindowProc(hWnd, uMsg, wParam, lParam);
            break;
    }

    return lRet;
}

GLboolean WinCreate(ESContext *esContext, const char *title) {
    WNDCLASS wndclass = {0};
    DWORD wStyle = 0;
    RECT windowRect;
    HINSTANCE hinstance = GetModuleHandle(NULL);

    wndclass.style = CS_OWNDC;
    wndclass.lpfnWndProc = (WNDPROC) ESWindowProc;
    wndclass.hInstance = hinstance;
    wndclass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wndclass.lpszClassName = "opengles3.0";

    if (!RegisterClass(&wndclass)) {
        return GL_FALSE;
    }

    wStyle = WS_VISIBLE | WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION;

    windowRect.left = 0;
    windowRect.top = 0;
    windowRect.right = esContext->width;
    windowRect.bottom = esContext->height;

    AdjustWindowRect(&windowRect, wStyle, FALSE);

    esContext->eglNativeWindow = CreateWindow(
            "opengles3.0",
            title,
            wStyle,
            0,
            0,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            NULL,
            NULL,
            hinstance,
            NULL
    );

#ifdef _WIN64
    SetWindowLongPtr(esContext->eglNativeWindow, GWL_USERDATA, (LONGLONG) (LONG_PTR) esContext);
#else
    SetWindowLongPtr ( esContext->eglNativeWindow, GWL_USERDATA, ( LONG ) ( LONG_PTR ) esContext );
#endif

    if (esContext->eglNativeWindow == NULL) {
        return GL_FALSE;
    }

    ShowWindow(esContext->eglNativeWindow, TRUE);

    return GL_TRUE;
}

void WinLoop(ESContext *esContext) {
    MSG msg = {0};
    int done = 0;
    DWORD lastTime = GetTickCount();

    while (!done) {
        int gotMsg = (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0);
        DWORD curTime = GetTickCount();
        float deltaTime = (float) (curTime - lastTime) / 1000.0f;
        lastTime = curTime;

        if (gotMsg) {
            if (msg.message == WM_QUIT) {
                done = 1;
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else {
            SendMessage(esContext->eglNativeWindow, WM_PAINT, 0, 0);
        }

        // Call update function if registered
        if (esContext->updateFunc != NULL) {
            esContext->updateFunc(esContext, deltaTime);
        }
    }
}

/** OpenGL ES Functions **/

EGLint GetContextRenderableType(EGLDisplay eglDisplay) {
#ifdef EGL_KHR_create_context
    const char *extensions = eglQueryString(eglDisplay, EGL_EXTENSIONS);
    if (extensions != NULL && strstr(extensions, "EGL_KHR_create_context")) {
        return EGL_OPENGL_ES3_BIT_KHR;
    }
#endif
    return EGL_OPENGL_ES2_BIT;
}

GLboolean ESUTIL_API esCreateWindow(ESContext *esContext, const char *title, GLint width, GLint height, GLuint flags) {
    EGLConfig config;
    EGLint majorVersion;
    EGLint minorVersion;
    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    if (esContext == NULL) {
        printf("ESContext is NULL in esCreateWindow\n");
        return GL_FALSE;
    }

    esContext->width = width;
    esContext->height = height;

    if (!WinCreate(esContext, title)) {
        printf("WinCreate return GL_FALSE\n");
        return GL_FALSE;
    }

    esContext->eglDisplay = eglGetDisplay(esContext->eglNativeDislpay);
    if (esContext->eglDisplay == EGL_NO_DISPLAY) {
        printf("EGL_NO_DISPLAY\n");
        return GL_FALSE;
    }

    if (!eglInitialize(esContext->eglDisplay, &majorVersion, &minorVersion)) {
        printf("eglInitialize return GL_FALSE\n");
        return GL_FALSE;
    }

    {
        EGLint numConfigs = 0;
        EGLint attribList[] = {
                EGL_RED_SIZE, 5,
                EGL_GREEN_SIZE, 6,
                EGL_BLUE_SIZE, 5,
                EGL_ALPHA_SIZE, (flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
                EGL_DEPTH_SIZE, (flags & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
                EGL_STENCIL_SIZE, (flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
                EGL_SAMPLE_BUFFERS, (flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
                EGL_RENDERABLE_TYPE, GetContextRenderableType(esContext->eglDisplay),
                EGL_NONE
        };

        if (!eglChooseConfig(esContext->eglDisplay, attribList, &config, 1, &numConfigs)) {
            printf("eglChooseConfig return GL_FALSE\n");
            return GL_FALSE;
        }

        if (numConfigs < 1) {
            printf("numConfigs less than 1\n");
            return GL_FALSE;
        }
    }

    // Create Window Surface
    esContext->eglSurface = eglCreateWindowSurface(esContext->eglDisplay, config,
                                                   esContext->eglNativeWindow, NULL);
    if (esContext->eglSurface == EGL_NO_SURFACE) {
        LOGE("EGL_NO_SURFACE\n");
        return GL_FALSE;
    }

    // Create EGL Context
    esContext->eglContext = eglCreateContext(esContext->eglDisplay, config,
                                             EGL_NO_CONTEXT, contextAttribs);

    if (esContext->eglContext == EGL_NO_CONTEXT) {
        LOGE("EGL_NO_CONTEXT\n");
        return GL_FALSE;
    }

    if (!eglMakeCurrent(esContext->eglDisplay, esContext->eglSurface,
                        esContext->eglSurface, esContext->eglContext)) {
        LOGE("eglMakeCurrent return GL_FALSE");
        return GL_FALSE;
    }
    return GL_TRUE;
}

void ESUTIL_API esRegisterDrawFunc(ESContext *esContext, void (ESCALLBACK *drawFunc)(ESContext *)) {
    esContext->drawFunc = drawFunc;
}

void ESUTIL_API esRegisterShutdownFunc(ESContext *esContext, void (ESCALLBACK *shutdownFunc)(ESContext *)) {
    esContext->shutdownFunc = shutdownFunc;
}

void ESUTIL_API esRegisterUpdateFunc(ESContext *esContext, void (ESCALLBACK *updateFunc)(ESContext *, float)) {
    esContext->updateFunc = updateFunc;
}

void
ESUTIL_API esRegisterKeyFunc(ESContext *esContext, void (ESCALLBACK *keyFunc)(ESContext *, unsigned char, int, int)) {
    esContext->keyFunc = keyFunc;
}

void ESUTIL_API esLogMessage(const char *formatStr, ...) {
    va_list params;
    char buf[BUFSIZ];

    printf("%s", buf);
            va_end(params);
}

static esFile *esFileOpen(void *ioContext, const char *fileName) {
    esFile *pFile = NULL;
    pFile = fopen(fileName, "rb");
    return pFile;
}

static void esFileClose(esFile *pFile) {
    if (pFile != NULL) {
        fclose(pFile);
        pFile = NULL;
    }
}

static int esFileRead(esFile *pFile, int bytesToRead, void *buffer) {
    int bytesRead = 0;
    if (pFile == NULL) {
        return bytesRead;
    }
}

char *ESUTIL_API esLoadTGA(void *ioContext, const char *fileName, int *width, int *height) {
    char *buffer;
    esFile *fp;
    TGA_HEADER Header;
    int bytesRead;

    fp = esFileOpen(ioContext, fileName);

    if (fp == NULL)
    {
        esLogMessage("esLoadTGA FAILED to load : { %s }\n", fileName);
        return NULL;
    }

    bytesRead = esFileRead(fp, sizeof(TGA_HEADER), &Header);

    *width = Header.Width;
    *height = Header.Height;

    if (Header.ColorDepth == 0 ||
        Header.ColorDepth == 24 || Header.ColorDepth == 32) {
        int bytesToRead = sizeof(char) * (*width) * (*height) * Header.ColorDepth / 8;

        buffer = (char *) malloc(bytesToRead);

        if (buffer) {
            bytesRead = esFileRead(fp, bytesToRead, buffer);
            esFileClose(fp);

            return (buffer);
        }
    }

    return (NULL);
}


/** Shader Functions **/

GLuint esLoadShader(GLenum shaderType, const char *pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char *buf = (char *) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %s: \n%s\n",
                         shaderType == GL_VERTEX_SHADER ? "Vertex Shader" : "Fragment Shader", buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint esCreateProgram(const char *pVertexSource, const char *pFragmentSource) {
    GLuint vertexShader = esLoadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader)
        return 0;
    GLuint pixelShader = esLoadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader)
        return 0;

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachVertexShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachPixelShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char *buf = (char *) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

void checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        switch (error) {
            case GL_INVALID_ENUM:
                LOGE("Operation: %s Error: 0x%x(%s)", op, error, "Invalid argument enum");
                break;
            case GL_INVALID_VALUE:
                LOGE("Operation: %s Error: 0x%x(%s)", op, error, "Invalid argument value");
                break;
            case GL_INVALID_OPERATION:
                LOGE("Operation: %s Error: 0x%x(%s)", op, error, "Invalid operation");
                break;
            case GL_OUT_OF_MEMORY:
                LOGE("Operation: %s Error: 0x%x(%s)", op, error, "Out Of Memory");
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                LOGE("Operation: %s Error: 0x%x(%s)", op, error, "Frame Buffer Error");
                break;
            default:
                LOGE("Operation: %s Error: 0x%x(%s)", op, error, "Unknown Error");
                break;
        }
    }
}

void printGlInfo() {
    LOGI("OpenGL ES: %-30s : %s", "GL Version", glGetString(GL_VERSION));
    LOGI("OpenGL ES: %-30s : %s", "GL Shader Version", glGetString(GL_SHADING_LANGUAGE_VERSION));
    LOGI("OpenGL ES: %-30s : %s", "GL Vender", glGetString(GL_VENDOR));
    LOGI("OpenGL ES: %-30s : %s", "GL Renderer", glGetString(GL_RENDERER));
    LOGI("OpenGL ES: %-30s : %s", "GL Extensions", glGetString(GL_EXTENSIONS));
}