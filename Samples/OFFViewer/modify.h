#ifndef MODIFY_H
#define MODIFY_H

#include <glm/matrix.hpp>
#include <glm/mat4x4.hpp>
#include <glm/common.hpp>
#include <glm/vec4.hpp>
#include "halfmesh.h"
#include <map>
#include <utility>
#include <tuple>

using namespace glm;

bool simpleMesh(Edge* & elist, Vertex* & vlist, Face* & flist, int* eNum, int* vNum, int* fNum, float limit)
{
    m_vert *nv = new m_vert[*vNum];
    m_face *nf = new m_face[*fNum];

    for (int i = 0; i < *vNum; i++)
    {
        nv[i].pos = vlist[i].position;
        nv[i].color = vlist[i].color;
    }
    for (int i = 0; i < *fNum; i++)
    {
        nf[i].color = flist[i].color;
        Edge *p = flist[i].edge;
        int j = 0;
        do 
        {
            nf[i].vid[j] = p->end - vlist;
            j++;
            p = p->next;
        }
        while (p != flist[i].edge);
        nf[i].size = j;
    }

    float avgLen = 0.f;
    float avgBase = 0.f;
    std::map<float, std::pair<Edge*, float> > emap;
    for (int i = 0; i < *eNum; i++)
    {
        Vertex *f[] = { elist[i].end, elist[i].next->end };
        float subcost[] = { 0.f, 0.f };
        bool succ = true;
        float lerp = 0.0f;
        float cost = 0.0f;
        float base = 0.f;
        vec3 dir = normalize(vec3(f[0]->position - f[1]->position));
        for (int j = 0; j < 2; j++)
        {
            int k = 16;
            Edge *p = f[j]->edge;
            do
            {
                if (!p->invert)
                {
                    succ = false;
                    break;
                }
                vec3 crs = cross(vec3(p->end->position - f[j]->position), 
                    vec3(p->invert->next->end->position - f[j]->position));
                avgLen += length(vec3(p->end->position - f[j]->position));
                avgLen += length(vec3(p->invert->next->end->position - f[j]->position));
                avgBase += 2.f;

                float w = abs(dot(normalize(crs), dir));
                subcost[j] += w;
                cost += w;
                base += 1.f;
                p = p->invert->next;
            }
            while (p != f[j]->edge && k-- > 0);
        }
        cost /= base;
        if (succ)
        {
            lerp = clamp(powf(subcost[1], .1f) / (powf(subcost[0], .1f) + powf(subcost[1], .1f)), 0.f, 1.f);
            if (cost < limit)
                emap.insert(std::make_pair(cost, std::make_pair(elist + i, lerp)));
        }
    }

    bool oneSim = false;
    if (emap.size() > 0)
    {
        for (auto it = emap.begin(); it != emap.end(); it++)
        {
            vec4 ctr = it->second.first->end->position * it->second.second + it->second.first->next->end->position * (1.f - it->second.second);
            it->second.first->end->position = ctr;
            it->second.first->next->end->position = ctr;
            it->second.first->end->color = vec3(1.0, 0.6, 0.2);
            it->second.first->next->end->color = vec3(1.0, 0.6, 0.2);
            nv[it->second.first->end - vlist].pos = ctr;
            nv[it->second.first->next->end - vlist].pos = ctr;
            oneSim = true;
            break;
        }
    }

    clean(nv, nf, vNum, fNum, 0.001 * avgLen / avgBase, 0.001 * avgLen / avgBase);

    loadMesh(nf, nv, *fNum, *vNum, vlist, flist, elist, eNum);
    delete[] nf;
    delete[] nv;
    return oneSim;
}

#endif