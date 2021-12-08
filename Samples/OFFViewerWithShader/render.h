#ifndef RENDER_H
#define RENDER_H

#include <GL/glew.h>
#include <cstdlib>
#include <cstdio>

GLuint loadShader(GLenum type, const char *path)
{
    FILE *f = fopen(path, "r");
    GLuint sdr = glCreateShader(type);
    GLchar* content = new GLchar[4096 * 1024];
    int seek = 0;
    int onceSize;
    do
    {
        onceSize = fread(content + seek, sizeof(GLchar), 1024, f);
        seek += onceSize;
    }
    while (onceSize);
    // printf("%s\n len: %d\n", content, seek);
    glShaderSource(sdr, 1, &content, &seek);
    glCompileShader(sdr);

    int logLength;
    char logs[1024];
    glGetShaderInfoLog(sdr, 1024, &logLength, logs);
    printf("[%s]\n", logs);
    
    delete[] content;
    return sdr;
}

#endif