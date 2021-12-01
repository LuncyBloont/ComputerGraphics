#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

#define SPEED 0.01f;
#define SPLIT4(p, k) p[0 + k * 4], p[1 + k * 4], p[2 + k * 4], p[3 + k * 4]
#define SPLIT3(p, k) p[0 + k * 3], p[1 + k * 3], p[2 + k * 3]

GLfloat movex = 0;
GLfloat movey = 0;

bool upToggle = false, downToggle = false, leftToggle = false, rightToggle = false;

GLfloat vertexs[] = {
    -0.5f, 0.0f, 0.0f,
    0.5f, 0.0f, 0.0f,
    0.0f, 0.5f, 0.0f};

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    GLfloat blue[] = {0.3f, 0.5f, 1.0f, 1.0f};
    GLfloat red[] = {0.9f, 0.5f, 0.2f, 1.0f};
    GLfloat green[] = {0.3f, 1.0f, 0.2f, 1.0f};

    vertexs[0] += movex;
    vertexs[3] += movex;
    vertexs[6] += movex;
    vertexs[1] += movey;
    vertexs[4] += movey;
    vertexs[7] += movey;

    glBegin(GL_TRIANGLES);
    glColor4f(SPLIT4(blue, 0));
    glVertex3f(SPLIT3(vertexs, 0));
    glColor4f(SPLIT4(red, 0));
    glVertex3f(SPLIT3(vertexs, 1));
    glColor4f(SPLIT4(green, 0));
    glVertex3f(SPLIT3(vertexs, 2));
    glEnd();
    glFlush();
    glutSwapBuffers();

    glutPostRedisplay();
}

void setup()
{
    glClearColor(0.3f, 0.2f, 0.1f, 1.0f);
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
        if (leftToggle) break;
        leftToggle = true;
        movex -= SPEED;
        break;
    case GLUT_KEY_RIGHT:
        if (rightToggle) break;
        rightToggle = true;
        movex += SPEED;
        break;
    case GLUT_KEY_UP:
        if (upToggle) break;
        upToggle = true;
        movey += SPEED;
        break;
    case GLUT_KEY_DOWN:
        if (downToggle) break;
        downToggle = true;
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
        leftToggle = false;
        break;
    case GLUT_KEY_RIGHT:
        movex -= SPEED;
        rightToggle = false;
        break;
    case GLUT_KEY_UP:
        movey -= SPEED;
        upToggle = false;
        break;
    case GLUT_KEY_DOWN:
        movey += SPEED;
        downToggle = false;
        break;
    default:
        break;
    }
}

int main(int argc, char **argv)
{
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