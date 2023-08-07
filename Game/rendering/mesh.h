#ifndef MESH_H
#define MESH_H

#include "renderer.h"

struct Mesh
{
    Mesh(renderer::VertexList vertices, renderer::VertexFormat vertexFormat, renderer::IndexList indices);
    ~Mesh();
    void draw() const;
    renderer::MeshHandle handle;
};

#endif // MESH_H
