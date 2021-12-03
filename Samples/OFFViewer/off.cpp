/*
 * Copyleft (c) 2021 Hy Xu.
 * Email: xhymaker@126.com
 * Models from: http://www.holmes3d.net/graphics/offfiles/ https://people.sc.fsu.edu/~jburkardt/data/off/off.html
 * 
 */

#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/common.hpp>
#include <glm/matrix.hpp>
#include <cstdio>
#include <map>
#include <utility>
#include <sstream>
#include "halfmesh.h"
#include "modify.h"

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

Face *faces;
Edge *edges;
Vertex *vertexs;
int faceNum;
int edgeNum;
int vertexNum;

int mouseX = 0, mouseY = 0;
bool mouseDown = false;
bool mouseDown2 = false;
bool loopActiveKeyDown = false;

bool flatRender = true;
bool outLine = true;

int viewVertexId = 0;

char *filePath;
bool withColor = false;

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

mat4x4 yaw = mat4x4(
    cosf(YAW_SPEED), 0.f, sinf(YAW_SPEED), 0.f,
    0.f, 1.f, 0.f, 0.f,
    -sinf(YAW_SPEED), 0.f, cosf(YAW_SPEED), 0.f,
    0.f, 0.f, 0.f, 1.f
);

mat4x4 pitch = mat4x4(
    1.f, 0.f, 0.f, 0.f,
    0.f, cosf(PITCH_SPEED), sinf(PITCH_SPEED), 0.f,
    0.f, -sinf(PITCH_SPEED), cosf(PITCH_SPEED), 0.f,
    0.f, 0.f, 0.f, 1.f
);

mat4x4 scale = mat4x4(
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

void drawVertex(Vertex *v, int size, mat4x4 trans)
{
    glPointSize(4.f);
    glPolygonOffset(-1.0, -3.0);
    glBegin(GL_POINTS);
    for (int i = 0; i < size; i++)
    {
        vec4 p = trans * v[i].position;
        glColor3f(SPLIT3(v[i].color));
        glVertex3f(SPLIT3(p));
    }
    glEnd();
    glPolygonOffset(0.0, 0.0);
}

void drawEdge(Edge *pre, int size, mat4x4 trans)
{
    glPolygonOffset(-1.0, -2.0);
    glLineWidth(2.);
    glBegin(GL_LINES);
    for (int i = 0; i < size; i++)
    {
        vec4 p1 = trans * pre[i].end->position;
        vec4 p2 = trans * pre[i].next->end->position;
        glColor3f(SPLIT3(pre[i].next->color));
        glVertex3f(SPLIT3(p1));
        glVertex3f(SPLIT3(p2));
    }
    glEnd();
    glPolygonOffset(0.0, 0.0);
}

void drawFace(Face *f, int size, mat4x4 trans)
{
    for (int i = 0; i < size; i++)
    {
        Edge *e = f[i].edge;
        vec4 p1, p2;
        int j = 0;
        glBegin(GL_TRIANGLE_FAN);
        do
        {
            vec4 p = trans * e->end->position;
            if (!flatRender)
            {
                glColor3f(SPLIT3((1.0 + dot(normalize(vec3(trans * e->end->normal)), vec3(0., 0., -1.))) / 2.0 * f[i].color));
                glVertex3f(SPLIT3(p));
                e = e->next;
            }
            else
            {
                if (j == 0) 
                {
                    p1 = p;
                    e = e->next;
                    j++;
                }
                else if (j == 1) 
                {
                    p2 = p;
                    e = e->next;
                    j++;
                }
                else
                {
                    vec3 normal = normalize(cross(vec3(p1) - vec3(p2), vec3(p) - vec3(p2)));
                    glColor3f(SPLIT3((1.0 + dot(normal, vec3(0., 0., -1.))) / 2.0 * f[i].color));
                    glVertex3f(SPLIT3(p2));
                    glVertex3f(SPLIT3(p));
                    j = 0;
                }
            }
        } while (e != f[i].edge || (flatRender && j != 0));
        glEnd();
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mat4x4 trans = transMat * moveMat * rotateMat;
    drawFace(faces, faceNum, trans);
    if (outLine) 
    {
        drawEdge(edges, edgeNum, trans);
        drawVertex(vertexs, vertexNum, trans);
    }

    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();
}

void activeLoop()
{
    if (viewVertexId < vertexNum)
    {
        for (int i = 0; i < vertexNum; i++)
        {
            vertexs[i].color = VERTEX_COLOR_UNACTIVE;
        }
        for (int i = 0; i < edgeNum; i++)
        {
            edges[i].color = LINE_COLOR_UNACTIVE;
        }
        vertexs[viewVertexId].color = VERTEX_COLOR_ACTIVE;

        Edge *e = vertexs[viewVertexId].edge;
        if (e->invert)
        {do 
            {
                e->color = LINE_COLOR_ACTIVE;
                e->invert->color = LINE_COLOR_ACTIVE;
                e = e->invert->next;
            }
            while (e != vertexs[viewVertexId].edge && e->invert);
        }
    }
}

float simLimit = 0.f;
void keyFunc(int key, int x, int y)
{
    if (key == GLUT_KEY_RIGHT && !loopActiveKeyDown)
    {
        loopActiveKeyDown = true;
        viewVertexId = ((viewVertexId + 1) % vertexNum + vertexNum) % vertexNum;
        activeLoop();
    }
    if (key == GLUT_KEY_LEFT && !loopActiveKeyDown)
    {
        loopActiveKeyDown = true;
        viewVertexId = ((viewVertexId - 1) % vertexNum + vertexNum) % vertexNum;
        activeLoop();
    }
    if (key == GLUT_KEY_DOWN)
    {
        for (int i = 0; i < edgeNum; i++)
        {
            if (!simpleMesh(edges, vertexs, faces, &edgeNum, &vertexNum, &faceNum, simLimit))
            {
                break;
            }
        }
    }
    if (key == GLUT_KEY_UP)
    {
        simLimit += 0.02f;
    }
}

std::string getLine(FILE *f)
{
    char line[1024];
    std::string lines;
    int i = 0;
    fgets(line, 1024, f);
    lines = line;
    if (lines.size() == 0) return "";
    for (; i < lines.size(); i++)
    {
        if (lines[i] != ' ') break;
    }
    if (i >= lines.size() || lines[i] == '#' || lines[i] == '\n')
    {
        return getLine(f);
    }
    return lines;
}

void loadModel(const char *fname, bool color, bool show)
{
    int v = 0, f, e;
    char type[32];
    FILE *mf = fopen(fname, "r");

    std::stringstream(getLine(mf)) >> type >> v >> f >> e;
    if (v == 0) std::stringstream(getLine(mf)) >> v >> f >> e;

    m_vert *nv = new m_vert[v];
    m_face *nf = new m_face[f];

    if (show) printf("Model info:\n    TYPE: %s\n    Raw Vertex: %d\n    Raw Face: %d\n    Raw Edge: %d\n", type, v, f, e);

    for (int i = 0; i < v; i++)
    {
        double x, y, z;
        std::stringstream(getLine(mf)) >> x >> y >> z;
        if (show) printf("-- vert%d (%lf, %lf, %lf)\n", i, x, y, z);
        nv[i].pos = vec4(x, y, z, 1.f);
        nv[i].color = VERTEX_COLOR_UNACTIVE;
    }

    for (int i = 0; i < f; i++)
    {
        int vn;
        int vpre = -1;
        int v0;

        auto strm = std::stringstream(getLine(mf));
        strm >> vn;
        
        if (show) printf("-- face{%d} [ ", i);
        nf[i].color = vec3(0.54, 0.55, 0.56);
        nf[i].size = vn;

        for (int j = 0; j < vn; j++)
        {
            int vi;
            strm >> vi;
            if (show) printf("%d ", vi);
            nf[i].vid[j] = vi;
        }
        if (show) printf("] ");
        if (color)
        {
            double r, g, b;
            strm >> r >> g >> b;
            nf[i].color = vec3(r, g, b);
            if (show) printf("COLOR: %g %g %g", r, g, b);
        }
        if (show) printf("\n");
    }

    faceNum = f;
    vertexNum = v;
    fclose(mf);

    loadMesh(nf, nv, f, v, vertexs, faces, edges, &edgeNum);
    delete[] nf;
    delete[] nv;
}

void keyUpFunc(int key, int x, int y)
{
    if (key == GLUT_KEY_RIGHT || key == GLUT_KEY_LEFT)
    {
        loopActiveKeyDown = false;
    }

    if (key == GLUT_KEY_UP)
    {
        
    }
}

void charKeyFunc(unsigned char key, int x, int y)
{
    if (key - '0' >= 0 && key - '0' <= 9)
    {
        viewVertexId *= 10;
        viewVertexId += (key - '0');
    }
    if (key == 'e')
    {
        activeLoop();
        viewVertexId = 0;
    }
    if (key == 'q')
    {
        for (int i = 0; i < vertexNum; i++)
        {
            vertexs[i].color = VERTEX_COLOR_UNACTIVE;
        }
        for (int i = 0; i < edgeNum; i++)
        {
            edges[i].color = LINE_COLOR_UNACTIVE;
        }
        viewVertexId = 0;
    }
    if (key == 'c')
    {
        if (glIsEnabled(GL_CULL_FACE)) glDisable(GL_CULL_FACE);
        else if (!glIsEnabled(GL_CULL_FACE)) glEnable(GL_CULL_FACE);
    }

    if (key == 'r')
    {
        loadModel(filePath, withColor, false);
        simLimit = 0.f;
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
        if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
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
                rotateMat = yaw * rotateMat;
            }
            for (int i = mouseX; i < x; i++)
            {
                rotateMat = inverse(yaw) * rotateMat;
            }
            for (int i = mouseY; i < y; i++)
            {
                rotateMat = pitch * rotateMat;
            }
            for (int i = y; i < mouseY; i++)
            {
                rotateMat = inverse(pitch) * rotateMat;
            }
        }
    }
    if (mouseDown2)
    {
        
        for (int i = mouseY; i < y; i++)
        {
            rotateMat = scale * rotateMat;
        }
        for (int i = y; i < mouseY; i++)
        {
            rotateMat = inverse(scale) * rotateMat;
        }
        for (int i = mouseX; i < x; i++)
        {
            rotateMat = scale * rotateMat;
        }
        for (int i = x; i < mouseX; i++)
        {
            rotateMat = inverse(scale) * rotateMat;
        }
    
    }
    mouseX = x;
    mouseY = y;
}

void checkModel(bool invW)
{
    for (int i = 0; i < vertexNum; i++)
    {
        if (vertexs[i].edge - edges < 0 || vertexs[i].edge - edges >= edgeNum)
        {
            printf("V%d lost edge\n", i);
        }
    }
    for (int i = 0; i < edgeNum; i++)
    {
        if (edges[i].belong - faces < 0 || edges[i].belong - faces >= faceNum)
        {
            printf("E%d lost face\n", i);
        }
        if (edges[i].next - edges < 0 || edges[i].next - edges >= edgeNum)
        {
            printf("E%d lost next\n", i);
        }
        if (edges[i].end - vertexs < 0 || edges[i].end - vertexs >= vertexNum)
        {
            printf("E%d lost end\n", i);
        }
        if (edges[i].invert - edges < 0 || edges[i].invert - edges >= edgeNum)
        {
            if (invW) printf("W: E%d lost invert\n", i);
        }
    }
    for (int i = 0; i < faceNum; i++)
    {
        if (faces[i].edge - edges < 0 || faces[i].edge - edges >= edgeNum)
        {
            printf("F%d lost edge\n", i);
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: off FILE [option]\noption:\n    c    model with face color\n");
        exit(-1);
    }
    loadModel(argv[1], argc > 2 && argv[2][0] == 'c', false);
    filePath = argv[1];
    withColor = argc > 2 && argv[2][0] == 'c';

    checkModel(false);

    glewInit();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OFF Model View");
    glutReshapeFunc([](int w, int h)
        {
            glLoadIdentity();
            gluPerspective(94., 4. / 3., 0.1, 1000.);
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
    });

    glClearColor(1.f, 1.f, 1.f, 1.f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glEnable(GL_POLYGON_OFFSET_POINT);

    glutMainLoop();

    return 0;
}