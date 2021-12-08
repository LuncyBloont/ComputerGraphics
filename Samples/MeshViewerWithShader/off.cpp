/*
 * Copyleft (c) 2021 Hy Xu.
 * Email: xhymaker@126.com
 * Models from: http://www.holmes3d.net/graphics/offfiles/ https://people.sc.fsu.edu/~jburkardt/data/off/off.html
 * 
 * 
 */

#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/common.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <map>
#include <utility>
#include <sstream>
#include "load.h"
#include "render.h"
#include "bmp.h"

#define YAW_SPEED 0.01f
#define PITCH_SPEED 0.003f
#define SCALE_SPEED 1.02f
#define MOVE_SPEED 0.004f

#define VERTEX_COLOR_ACTIVE vec3(0.6f, 0.8f, 1.0f)
#define VERTEX_COLOR_UNACTIVE vec3(0.2f, 0.4f, 0.8f)
#define LINE_COLOR_ACTIVE vec3(0.9f, 0.3f, 0.2f)
#define LINE_COLOR_UNACTIVE vec3(0.4f, 0.5f, 0.6f)

#define SPLIT3(v) v.x, v.y, v.z
#define SPLIT4(v) v.x, v.y, v.z, v.w

using namespace glm;

m_vert *verts = nullptr;
m_face *faces = nullptr;
int faceNum = 0;
int vertexNum = 0;

GLuint shaderProgram;

int mouseX = 0, mouseY = 0;
bool mouseDown = false;
bool mouseDown2 = false;

bool flatRender = true;
bool outLine = true;

char *filePath;
bool sKey = false;

bmp_head texData;
bmp_head aoTexData;
bmp_head smoothTexData;

mat4x4 transMat = mat4x4(
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, -1.f, 1.f
);

mat4x4 rotateMat = mat4x4(
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f
);

mat4x4 moveMat = mat4x4(
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f
);

mat4x4 ryaw = mat4x4(
    cosf(YAW_SPEED), 0.f, sinf(YAW_SPEED), 0.f,
    0.f, 1.f, 0.f, 0.f,
    -sinf(YAW_SPEED), 0.f, cosf(YAW_SPEED), 0.f,
    0.f, 0.f, 0.f, 1.f
);

mat4x4 rpitch = mat4x4(
    1.f, 0.f, 0.f, 0.f,
    0.f, cosf(PITCH_SPEED), sinf(PITCH_SPEED), 0.f,
    0.f, -sinf(PITCH_SPEED), cosf(PITCH_SPEED), 0.f,
    0.f, 0.f, 0.f, 1.f
);

mat4x4 mscale = mat4x4(
    SCALE_SPEED, 0.f, 0.f, 0.f,
    0.f, SCALE_SPEED, 0.f, 0.f,
    0.f, 0.f, SCALE_SPEED, 0.f,
    0.f, 0.f, 0.f, 1.f
);

mat4x4 moveUp = mat4x4(
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, MOVE_SPEED, 0.f, 1.f
);

mat4x4 moveRight = mat4x4(
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    MOVE_SPEED, 0.f, 0.f, 1.f
);

mat4x4 genProjection(GLfloat znear, GLfloat zfar, GLfloat width, GLfloat height)
{
    return mat4x4(
        2.f * znear / width, 0.f, 0.f, 0.f,
        0.f, 2.f * znear / height, 0.f, 0.f,
        0.f, 0.f, (zfar + znear) / (znear - zfar), 2.f * zfar * znear / (znear - zfar),
        0.f, 0.f, -1.f, 0.f
    );
}

GLuint vaoID;
GLuint bufID;
#define RESET 21997799
#define FORMAT (3 + 2 + 3)
GLuint *findex = nullptr;
GLuint findexSize;
GLfloat *nvraw = nullptr;
GLuint mainTexture, aoTexture, smoothTexture;

void indexVF()
{
    if (findex) delete[] findex;
    if (nvraw) delete[] nvraw;
    nvraw = new GLfloat[vertexNum * FORMAT]; // pos, uv, normal
    findex = new GLuint[vertexNum + faceNum];
    GLfloat minp = 1e9f;
    GLfloat maxp = -1e9f;
    for (int i = 0; i < vertexNum; i++)
    {
        nvraw[i * FORMAT + 0] = verts[i].pos.x;
        nvraw[i * FORMAT + 1] = verts[i].pos.y;
        nvraw[i * FORMAT + 2] = verts[i].pos.z;

        nvraw[i * FORMAT + 5] = verts[i].normal.x;
        nvraw[i * FORMAT + 6] = verts[i].normal.y;
        nvraw[i * FORMAT + 7] = verts[i].normal.z;

        nvraw[i * FORMAT + 3] = verts[i].uv.x;
        nvraw[i * FORMAT + 4] = verts[i].uv.y;
    }
    int ix = 0;
    for (int i = 0; i < faceNum; i++)
    {
        for (int j = 0; j < faces[i].size; j++)
        {
            findex[ix] = faces[i].vid[j];
            ix++;
        }
        findex[ix] = RESET;
        ix++;
    }
    findexSize = ix;
}

void init()
{
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);
    GLfloat triny[] = {
        0.5f, 0.2f, 0.f, 0.f, 0.f,
        -0.5f, 0.2f, 0.f, 1.f, 0.f,
        0.f, -0.5f, 0.f, 1.f, 1.f
    };

    glGenBuffers(1, &bufID);
    glBindBuffer(GL_ARRAY_BUFFER, bufID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * FORMAT * vertexNum, nvraw, GL_STATIC_DRAW);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenTextures(1, &mainTexture);
    glBindTexture(GL_TEXTURE_2D, mainTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, UNPACK(texData.width, 32), UNPACK(texData.height, 32), 0,
        GL_BGR, GL_UNSIGNED_BYTE, texData.data + UNPACK(texData.offset, 32));
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glGenTextures(1, &aoTexture);
    glBindTexture(GL_TEXTURE_2D, aoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, UNPACK(aoTexData.width, 32), UNPACK(aoTexData.height, 32), 0,
        GL_BGR, GL_UNSIGNED_BYTE, aoTexData.data + UNPACK(aoTexData.offset, 32));
    glGenerateMipmap(GL_TEXTURE_2D);

    glGenTextures(1, &smoothTexture);
    glBindTexture(GL_TEXTURE_2D, smoothTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, UNPACK(smoothTexData.width, 32), UNPACK(smoothTexData.height, 32), 0,
        GL_BGR, GL_UNSIGNED_BYTE, smoothTexData.data + UNPACK(smoothTexData.offset, 32));
    glGenerateMipmap(GL_TEXTURE_2D);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mat4x4 ptrans = transMat * moveMat * rotateMat;
    mat4x4 ntrans = transpose(inverse(transMat * rotateMat));
    mat4x4 proj = genProjection(2.f, -1500.f, .8f, .6f);

    glUseProgram(shaderProgram);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, bufID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * FORMAT, reinterpret_cast<void*>(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * FORMAT, reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * FORMAT, reinterpret_cast<void*>(5 * sizeof(GLfloat)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mainTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, aoTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, smoothTexture);

    glUniform1i(glGetUniformLocation(shaderProgram, "mainTex"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "aoTex"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "smoothTex"), 2);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "trans"), 
        1, GL_FALSE, value_ptr(ptrans));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ntrans"), 
        1, GL_FALSE, value_ptr(ntrans));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projec"), 
        1, GL_FALSE, value_ptr(proj));

    
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(RESET);
    glPointSize(3.f);
    if (!outLine)
    {
        glDrawElements(GL_TRIANGLE_FAN, findexSize, GL_UNSIGNED_INT, findex);
    }
    else
    {
        glDrawElements(GL_LINE_LOOP, findexSize, GL_UNSIGNED_INT, findex);
        glDrawElements(GL_POINTS, findexSize, GL_UNSIGNED_INT, findex);
    }
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();
}

void keyFunc(int key, int x, int y)
{
   
}

void keyUpFunc(int key, int x, int y)
{
    if (key == GLUT_KEY_RIGHT || key == GLUT_KEY_LEFT)
    {
        
    }

    if (key == GLUT_KEY_UP)
    {
        
    }
}

void charKeyFunc(unsigned char key, int x, int y)
{
    if (key == 'c')
    {
        if (glIsEnabled(GL_CULL_FACE)) glDisable(GL_CULL_FACE);
        else if (!glIsEnabled(GL_CULL_FACE)) glEnable(GL_CULL_FACE);
    }
    if (key == 's')
    {
        sKey = true;
    }
}

void mouseFunc(int but, int sts, int x, int y)
{
    if (but == GLUT_LEFT_BUTTON && sts == GLUT_DOWN)
    {
        mouseDown = true;
    }
    if (but == GLUT_LEFT_BUTTON && sts == GLUT_UP)
    {
        mouseDown = false;
    }
    if (but == GLUT_RIGHT_BUTTON && sts == GLUT_DOWN)
    {
        mouseDown2 = true;
    }
    if (but == GLUT_RIGHT_BUTTON && sts == GLUT_UP)
    {
        mouseDown2 = false;
    }
    mouseX = x;
    mouseY = y;
}

void motionFunc(int x, int y)
{
    if (mouseDown)
    {
        if (sKey)
        {
            for (int i = y; i < mouseY; i++)
            {
                moveMat = moveUp * moveMat;
            }
            for (int i = mouseY; i < y; i++)
            {
                moveMat = inverse(moveUp) * moveMat;
            }
            for (int i = mouseX; i < x; i++)
            {
                moveMat = moveRight * moveMat;
            }
            for (int i = x; i < mouseX; i++)
            {
                moveMat = inverse(moveRight) * moveMat;
            }
        }
        else
        {
            for (int i = x; i < mouseX; i++)
            {
                rotateMat = ryaw * rotateMat;
            }
            for (int i = mouseX; i < x; i++)
            {
                rotateMat = inverse(ryaw) * rotateMat;
            }
            for (int i = mouseY; i < y; i++)
            {
                rotateMat = rpitch * rotateMat;
            }
            for (int i = y; i < mouseY; i++)
            {
                rotateMat = inverse(rpitch) * rotateMat;
            }
        }
    }
    if (mouseDown2)
    {
        
        for (int i = mouseY; i < y; i++)
        {
            rotateMat = mscale * rotateMat;
        }
        for (int i = y; i < mouseY; i++)
        {
            rotateMat = inverse(mscale) * rotateMat;
        }
        for (int i = mouseX; i < x; i++)
        {
            rotateMat = mscale * rotateMat;
        }
        for (int i = x; i < mouseX; i++)
        {
            rotateMat = inverse(mscale) * rotateMat;
        }
    
    }
    mouseX = x;
    mouseY = y;
}

int main(int argc, char **argv)
{
    if (argc < 5)
    {
        printf("Usage: off FILE TEXFILE AO SMOOTH\n");
        exit(-1);
    }
    load(verts, faces, &vertexNum, &faceNum, argv[1]);
    indexVF();
    texData = readBmp(argv[2]);
    aoTexData = readBmp(argv[3]);
    smoothTexData = readBmp(argv[4]);
    filePath = argv[1];

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OFF Model View");
    glutReshapeFunc([](int w, int h)
        {
            glViewport(0, 0, w, h);
        });
    glutDisplayFunc(display);
    glutSpecialFunc(keyFunc);
    glutSpecialUpFunc(keyUpFunc);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);
    glutKeyboardFunc(charKeyFunc);
    glutKeyboardUpFunc([](unsigned char k, int x, int y) { 
        if (k == 'f')
        {
            flatRender = !flatRender;
        }
        if (k == 'l')
        {
            outLine = !outLine;
        }
        if (k == 's')
        {
            sKey = false;
        }
    });

    glClearColor(0.3f, 0.2f, 0.1f, 1.f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glEnable(GL_POLYGON_OFFSET_POINT);
    glewInit();

    GLuint vertShader = loadShader(GL_VERTEX_SHADER, "./shader/blph.vert");
    GLuint fragShader = loadShader(GL_FRAGMENT_SHADER, "./shader/blph.frag");
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    int logLen;
    char logs[1024];
    glGetProgramInfoLog(shaderProgram, 1024, &logLen, logs);
    printf("[%s]\n", logs);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    init();
    glutMainLoop();

    return 0;
}