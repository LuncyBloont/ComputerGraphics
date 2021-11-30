#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <GLTools.h>

#define SPEED 0.001f;

GLBatch triangle;
GLShaderManager shaderManager;

GLfloat movex = 0;
GLfloat movey = 0;

GLfloat vertexs[] = {
    -0.5f, 0.0f, 0.0f,
    0.5f, 0.0f, 0.0f,
    0.0f, 0.5f, 0.0f};

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    GLfloat blue[] = {0.3f, 0.5f, 1.0f, 1.0f};
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, blue);

    triangle.Draw();

    glutSwapBuffers();

    vertexs[0] += movex;
    vertexs[3] += movex;
    vertexs[6] += movex;
    vertexs[1] += movey;
    vertexs[4] += movey;
    vertexs[7] += movey;

    triangle.CopyVertexData3f(vertexs);
    glutPostRedisplay();
}

void setup()
{
    glClearColor(0.3f, 0.2f, 0.1f, 1.0f);
    shaderManager.InitializeStockShaders();

    triangle.Begin(GL_TRIANGLES, 3);
    triangle.CopyVertexData3f(vertexs);
    triangle.End();
}

void resizeWindow(int w, int h)
{
    glViewport(0, 0, w, h);
}

void inputFunc(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        movex -= SPEED;
        break;
    case GLUT_KEY_RIGHT:
        movex += SPEED;
        break;
    case GLUT_KEY_UP:
        movey += SPEED;
        break;
    case GLUT_KEY_DOWN:
        movey -= SPEED;
        break;
    default:
        break;
    }
}

void inputUpFunc(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        movex += SPEED;
        break;
    case GLUT_KEY_RIGHT:
        movex -= SPEED;
        break;
    case GLUT_KEY_UP:
        movey -= SPEED;
        break;
    case GLUT_KEY_DOWN:
        movey += SPEED;
        break;
    default:
        break;
    }
}

int main(int argc, char **argv)
{
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Triangle");
    glutReshapeFunc(resizeWindow);
    glutDisplayFunc(display);
    glutSpecialFunc(inputFunc);
    glutSpecialUpFunc(inputUpFunc);
    glewInit();

    setup();

    GLenum err;
    do
    {
        err = glGetError();
        printf("%d\n", err);
    } while (err != GL_NO_ERROR);

    glutMainLoop();
    return 0;
}