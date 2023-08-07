#ifndef MESH_H
#define MESH_H

#include "renderer.h"

struct Mesh
{
    Mesh(renderer::VertexList vertices, renderer::VertexFormat vertexFormat, renderer::IndexList indices);
    void free();
    void draw() const;
    renderer::MeshHandle handle;

    static Mesh triangle();
};

#endif // MESH_H
