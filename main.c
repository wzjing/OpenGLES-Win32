#include <stdio.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include "util/gl-util.h"
#include "util/win-util.h"
#include "util/native-log.h"
#include <time.h>

#define WIDTH 1280
#define HEIGHT 720

typedef struct {
    // Handle to a program object
    GLuint programObject;

} UserData;

GLint time_handle;
GLint resolution_handle;

///
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader(GLenum type, const char *shaderSrc) {
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader(type);

    if (shader == 0) {
        return 0;
    }

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            char *infoLog = malloc(sizeof(char) * infoLen);

            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            loge("Error compiling shader:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;

}

///
// Initialize the shader and program object
//
int Init(ESContext *esContext) {
    UserData *userData = esContext->userData;
    char vShaderStr[] =
            "#version 300 es                          \n"
            "layout(location = 0) in vec4 vPosition;  \n"
            "void main()                              \n"
            "{                                        \n"
            "   gl_Position = vPosition;              \n"
            "}                                        \n";

//    char fShaderStr[] =
//            "#version 300 es                              \n"
//            "precision mediump float;                     \n"
//            "out vec4 fragColor;                          \n"
//            "void main()                                  \n"
//            "{                                            \n"
//            "   fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"
//            "}                                            \n";
    char *fShaderStr;
    load_file(&fShaderStr, "sea.glsl");

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;

    // Load the vertex/fragment shaders
    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    // Create the program object
    programObject = glCreateProgram();

    if (programObject == 0) {
        return 0;
    }

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Link the program
    glLinkProgram(programObject);

    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint infoLen = 0;

        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            char *infoLog = malloc(sizeof(char) * infoLen);

            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            loge("Error linking program:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteProgram(programObject);
        return FALSE;
    }

    time_handle = glGetUniformLocation(programObject, "iGlobalTime");
    resolution_handle = glGetUniformLocation(programObject, "iResolution");

    // Store the program object
    userData->programObject = programObject;

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return TRUE;
}

///
// Draw a triangle using the shader pair created in Init()
//
void Draw(ESContext *esContext) {
    UserData *userData = esContext->userData;
    GLfloat vVertices[] = {
            -1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f,1.0f,0.0f};

    // Set the viewport
    glViewport(0, 0, esContext->width, esContext->height);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the program object
    glUseProgram(userData->programObject);

    glUniform1f(time_handle, (float) clock() / CLOCKS_PER_SEC);
    glUniform2f(resolution_handle, WIDTH, HEIGHT);

    // Load the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Shutdown(ESContext *esContext) {
    UserData *userData = esContext->userData;

    glDeleteProgram(userData->programObject);
}

int esMain(ESContext *esContext) {
    esContext->userData = malloc(sizeof(UserData));

    if (!esCreateWindow(esContext, "Hello Triangle", WIDTH, HEIGHT, ES_WINDOW_RGB)) {
        printf("esCreateWindow return  GL_FALSE\n");
        return GL_FALSE;
    }

    if (!Init(esContext)) {
        return GL_FALSE;
    }

    esRegisterShutdownFunc(esContext, Shutdown);
    esRegisterDrawFunc(esContext, Draw);

    return GL_TRUE;
}

int main() {
    ESContext esContext;

    memset(&esContext, 0, sizeof(ESContext));

    if (esMain(&esContext) != GL_TRUE) {
        printf("esMain return GL_FALSE\n");
        return 1;
    }

    WinLoop(&esContext);

    if (esContext.shutdownFunc != NULL) {
        esContext.shutdownFunc(&esContext);
    }

    if (esContext.userData != NULL) {
        free(esContext.userData);
    }
    printf("Running");
    return 0;
}