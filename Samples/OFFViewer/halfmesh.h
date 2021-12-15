#ifndef HALFMESH_H
#define HALFMESH_H

#include <glm/matrix.hpp>
#include <glm/mat4x4.hpp>
#include <glm/common.hpp>
#include <glm/vec4.hpp>
#include <map>
#include <utility>

using namespace glm;

struct Face;
struct Edge;
struct Vertex;

struct Vertex
{
    vec4 position;
    Edge *edge = nullptr;
    vec3 color;
    vec4 normal;
};

struct Edge
{
    Vertex *end = nullptr;
    Edge *next = nullptr;
    Edge *invert = nullptr;
    Face *belong = nullptr;
    vec3 color;
};

struct Face
{
    Edge *edge = nullptr;
    vec3 color;
};

struct m_vert
{
    vec4 pos;
    vec3 color;
};

struct m_face
{
    int vid[100];
    int size = 0;
    vec3 color;
};

void loadMesh(m_face *fs, m_vert *vs, int fc, int vc, Vertex *&vertexs, Face *&faces, Edge *&edges, int *ecnt)
{
    int v = 0, f, e = 0;
    int ei = 0;
    std::map<std::pair<int, int>, int> edgeSheet; // (Vi, Vj) : Ex

    v = vc;
    f = fc;

    vertexs = new Vertex[v];
    faces = new Face[f];
    e = max(e, v + f - 2) + 1000;
    edges = new Edge[e * 2];

    for (int i = 0; i < v; i++)
    {
        vertexs[i].position = vs[i].pos;
        vertexs[i].color = vs[i].color;
    }

    for (int i = 0; i < f; i++)
    {
        int vn;
        int vpre = -1;
        int v0;

        vn = fs[i].size;

        faces[i].color = fs[i].color;
        int vcnt = 0;
        for (int j = 0; j <= vn; j++)
        {
            int vi = v0;
            if (j < vn)
                vi = fs[i].vid[j];
            vcnt++;

            // 流形要求，该点与该面的上一个点的正边一定是第一次出现
            if (vpre == -1)
            {
                // 这是第一个点
                v0 = vi;
                vpre = vi;
                faces[i].edge = edges + ei;
            }
            else
            {
                // 与上一个点构成边，该边终点为当前点，即边 vpre->vi
                // new edge (end: vi)
                // vpre (out edge: new edge)

                edges[ei].end = vertexs + vi;
                edges[ei].belong = faces + i;
                vertexs[vpre].edge = edges + ei;
                edges[ei].next = (vi == v0 ? vertexs[v0].edge : edges + ei + 1);
                edgeSheet.insert(std::make_pair(std::make_pair(vpre, vi), ei));
                if (edgeSheet.find(std::make_pair(vi, vpre)) != edgeSheet.end())
                {
                    edges[ei].invert = edges + edgeSheet[std::make_pair(vi, vpre)];
                    edges[edgeSheet[std::make_pair(vi, vpre)]].invert = edges + ei;
                }
                edges[ei].color = vec3(0.3, 0.5, 0.8);
                ei += 1;
                vpre = vi;
            }
        }
    }

    for (int i = 0; i < fc; i++)
    {
        Edge *p = faces[i].edge;
        if (p == p->next->next) continue;
        vec3 nor = normalize(cross(
            vec3(p->end->position - p->next->end->position),
            vec3(p->next->end->position - p->next->next->end->position)));
        do
        {
            p->end->normal -= vec4(nor, 0.f);
            p = p->next;
        } while (p != faces[i].edge);
    }

    /*for (int i = 0; i < vc; i++)
    {
        vec3 normal = vec3(0.f);
        float base = 0.f;
        bool valid = false;
        Edge *p = vertexs[i].edge;
        int k = 100;
        do
        {
            if (!p || !p->invert)
                break;
            valid = true;
            vec3 v1 = vec3(p->end->position - vertexs[i].position);
            vec3 v2 = vec3(p->invert->next->end->position - vertexs[i].position);

            normal += normalize(cross(v1, v2));
            base += 1.f;

            p = p->invert->next;
        } while (p != vertexs[i].edge && k-- > 0);

        if (valid)
        {
            vertexs[i].normal = vec4(normal / base, 0.f);
        }
    }*/

    *ecnt = ei;
}

void clean(m_vert *vs, m_face *fs, int *vcnt, int *fcnt, float slimit, float llimit)
{
    int *replace = new int[*vcnt];
    for (int i = 0; i < *vcnt; i++)
        replace[i] = i;
    for (int i = 0; i < *fcnt; i++)
    {
        for (int j = 0; j < fs[i].size; j++)
        {
            int v1 = fs[i].vid[j];
            int v2 = j + 1 < fs[i].size ? fs[i].vid[j + 1] : fs[i].vid[0];
            if (distance(vec3(vs[v1].pos), vec3(vs[v2].pos)) < llimit)
            {
                replace[v1] = replace[v2];
            }
        }
    }
    for (int i = 0; i < *fcnt; i++)
    {
        for (int j = 0; j < fs[i].size; j++)
        {
            fs[i].vid[j] = replace[fs[i].vid[j]];
        }
        for (int j = 0; j < fs[i].size; j++)
        {
            if (fs[i].vid[j] == fs[i].vid[(j + 1) % fs[i].size])
            {
                for (int k = j; k < fs[i].size - 1; k++)
                {
                    fs[i].vid[k] = fs[i].vid[k + 1];
                }
                fs[i].size--;
            }
        }
    }

    delete[] replace;
}

#endif
