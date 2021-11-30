#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <GLTools.h>

GLBatch triangle;
GLShaderManager shaderManager;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    GLfloat blue[] = { 0.3f, 0.5f, 1.0f, 1.0f };
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, blue);

    triangle.Draw();

    glutSwapBuffers();
}

void setup()
{
    glClearColor(0.3f, 0.2f, 0.1f, 1.0f);
    shaderManager.InitializeStockShaders();
    GLfloat vertexs[] = {
        -0.5f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    triangle.Begin(GL_TRIANGLES, 3);
    triangle.CopyVertexData3f(vertexs);
    triangle.End();
}

void resizeWindow(int w, int h)
{
    glViewport(0, 0, w, h);
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
    glewInit();

    setup();

    GLenum err;
    do
    {
        err = glGetError();
        printf("%d\n", err);
    }
    while (err != GL_NO_ERROR);

    
    glutMainLoop();
    return 0;
}