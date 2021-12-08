#ifndef LOAD_H
#define LOAD_H

#include <glm/matrix.hpp>
#include <glm/mat4x4.hpp>
#include <glm/common.hpp>
#include <glm/vec4.hpp>
#include <map>
#include <utility>
#include <GL/glew.h>
#include <cstdio>
#include <cstring>
#include <sstream>

using namespace glm;

struct m_vert
{
    vec3 pos;
    vec2 uv;
    vec3 normal;
};

struct m_face
{
    int vid[100];
    int size = 0;
};

void load(m_vert* & nv, m_face* & nf, int* vc, int* fc, const char* fname)
{
    char buffer[4096];
    FILE *f = fopen(fname, "r");
    if (!f)
    {
        printf("File open failed.\n");
        exit(-1);
    }
    fgets(buffer, 4096, f);
    sscanf(buffer, "%d %d", vc, fc);
    if (nv) delete[] nv;
    if (nf) delete[] nf;
    nv = new m_vert[*vc];
    nf = new m_face[*fc];

    for (int i = 0; i < *vc; i++)
    {
        GLfloat x, y, z, u, v, nx, ny, nz;
        fgets(buffer, 4096, f);
        sscanf(buffer, "%f %f %f %f %f %f %f %f", &x, &y, &z, &u, &v, &nx, &ny, &nz);
        nv[i].pos = vec3(x, y, z);
        nv[i].uv = vec2(u, v);
        nv[i].normal = vec3(nx, ny, nz);
    }
    for (int i = 0; i < *fc; i++)
    {
        int n;
        int v;
        fgets(buffer, 4096, f);
        std::stringstream stream(buffer);
        stream >> n;
        nf[i].size = 0;
        for (int j = 0; j < n; j++)
        {
            stream >> v;
            nf[i].vid[nf[i].size] = v;
            nf[i].size++;
        }
    }
}

#endif